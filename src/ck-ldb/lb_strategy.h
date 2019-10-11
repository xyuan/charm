#ifndef LB_STRATEGY_H
#define LB_STRATEGY_H

#include <random>
#include <algorithm>

namespace lb_strategy
{

  // The reason for having pointer and non-pointer versions of these is so that
  // load balancing strategies can accept both vectors of objects and vectors
  // of pointers to objects. My tests always showed that strategies run faster
  // passing a vector of objects instead of pointers, but it doesn't hurt to
  // keep this around for flexibility

  template <typename T, bool is_ptr = std::is_pointer<T>::value> struct CmpLoadGreater {};

  template <typename T> struct CmpLoadGreater<T,true> {
    bool operator()(const T a, const T b) const { return (a->getLoad() > b->getLoad()); }
  };

  template <typename T> struct CmpLoadGreater<T,false> {
    bool operator()(const T &a, const T &b) const { return (a.getLoad() > b.getLoad()); }
  };

  template <typename T, bool is_ptr = std::is_pointer<T>::value> struct CmpLoadLess {};

  template <typename T> struct CmpLoadLess<T,true> {
    bool operator()(const T a, const T b) const { return (a->getLoad() < b->getLoad()); }
  };

  template <typename T> struct CmpLoadLess<T,false> {
    bool operator()(const T &a, const T &b) const { return (a.getLoad() < b.getLoad()); }
  };

  template <typename T, bool is_ptr = std::is_pointer<T>::value> struct CmpId {};

  template <typename T> struct CmpId<T,true> {
    bool operator()(const T a, const T b) const { return (a->id < b->id); }
  };

  template <typename T> struct CmpId<T,false> {
    bool operator()(const T &a, const T &b) const { return (a.id < b.id); }
  };

  template <typename T>
  T *ptr(T &obj) { return &obj; }  // turn reference into pointer

  template <typename T>
  T *ptr(T *obj) { return obj; }  // obj is already pointer, return it

  // ---------------- Obj --------------------

  // Obj represents a migratable chare with a load (1D) or vector of loads (nD).
  // Load balancing strategies typically receive a list of Obj and decide
  // where to migrate them

  struct obj_1_data { float load; };  // data for objects with non-vector load
  template <int N> struct obj_N_data { float load[N]; float maxload; };

  template <int N, bool multi = (N > 1)>
  class Obj : public std::conditional<multi, obj_N_data<N>, obj_1_data>::type
  {
  public:

    unsigned int id;
    int oldPe; // PE on which this object lives when load balancing was called

    void populate(unsigned int _id, float *_load, int _oldPe) {
      id = _id;
      oldPe = _oldPe;
      this->maxload = 0;
      for (int i=0; i < N; i++) {
        this->load[i] = _load[i];
        if (this->load[i] > this->maxload) this->maxload = this->load[i];
      }
    }

    float getLoad() const { return this->maxload; }
  };

  template<> inline void Obj<1>::populate(unsigned int _id, float *_load, int _oldPe) {
    id = _id; load = *_load; oldPe = _oldPe;
  }

  template<> inline float Obj<1>::getLoad() const { return load; }

  // ------------------ Proc ------------------

  /**
    * class Proc<int N, bool rateAware>
    * This just shows the interface. only the specializations (below) are defined.
    * N: number of dimensions of vector load
    * rateAware: true if speed aware, false otherwise
    */
  template <int N, bool rateAware, bool multi = (N > 1)>
  class Proc
  {
  public:
    void populate(int _id, float *_bgload, float *_speed);
    float getLoad() const;         // returns current load of processor
    void assign(const Obj<N> *o);  // add object loads to this processor's loads
    void assign(const Obj<N> &o);  // add object loads to this processor's loads
    void resetLoad();              // sets processor loads to background loads
  };

  template <int N> struct proc_N_data { float load[N] = {0}; float bgload[N] = {0}; float maxload = 0; };
  struct proc_1_data { float load = 0; float bgload = 0; };

  // --------- Proc rateAware=false specializations ---------

  template <int N, bool multi>
  class Proc<N,false,multi> : public std::conditional<multi, proc_N_data<N>, proc_1_data>::type
  {
  public:
    int id = -1;

    void populate(int _id, float *_bgload, float *_speed) {
      id = _id;
      std::copy_n(_bgload, N, this->bgload);
    }

    float getLoad() const { return this->maxload; }

    void assign(const Obj<N> *o) {
      for (int i=0; i < N; i++) {
        this->load[i] += o->load[i];
        if (this->load[i] > this->maxload) this->maxload = this->load[i];
      }
    }
    void assign(const Obj<N> &o) { assign(&o); }

    void resetLoad() {
      this->maxload = 0;
      for (int i=0; i < N; i++) {
        this->load[i] = this->bgload[i];
        if (this->load[i] > this->maxload) this->maxload = this->load[i];
      }
    }
  };

  template <> inline void Proc<1,false>::populate(int _id, float *_bgload, float *_speed) {
    id = _id; this->bgload = *_bgload;
  }
  template <> inline float Proc<1,false>::getLoad() const { return this->load; }
  template <> inline void Proc<1,false>::assign(const Obj<1> *o) { this->load += o->load; }
  template <> inline void Proc<1,false>::resetLoad() { this->load = this->bgload; }

  // --------- Proc rateAware=true specializations ---------

  // TODO further specialize for N=1 by having speed not be an array?

  template <int N, bool multi>
  class Proc<N,true,multi> : public std::conditional<multi, proc_N_data<N>, proc_1_data>::type
  {
  public:
    int id = -1;
    float speed[N] = {1.0};

    void populate(int _id, float *_bgload, float *_speed) {
      id = _id;
      std::copy_n(_bgload, N, this->bgload);
      std::copy_n(_speed, N, this->speed);
    }

    float getLoad() const { return this->maxload; }

    void assign(const Obj<N> *o) {
      for (int i=0; i < N; i++) {
        this->load[i] += (o->load[i] / speed[i]);
        if (this->load[i] > this->maxload) this->maxload = this->load[i];
      }
    }
    void assign(const Obj<N> &o) { assign(&o); }

    void resetLoad() {
      this->maxload = 0;
      for (int i=0; i < N; i++) {
        this->load[i] = this->bgload[i];
        if (this->load[i] > this->maxload) this->maxload = this->load[i];
      }
    }
  };

  template <> inline void Proc<1,true>::populate(int _id, float *_bgload, float *_speed) {
    id = _id; this->bgload = *_bgload; speed[0] = _speed[0];
  }
  template <> inline float Proc<1,true>::getLoad() const { return this->load; }
  template <> inline void Proc<1,true>::assign(const Obj<1> *o) { this->load += (o->load / speed[0]); }
  template <> inline void Proc<1,true>::resetLoad() { this->load = this->bgload; }

  // ---------------- Strategy --------------------

  template <typename O, typename P, typename S>
  class Strategy
  {
    public:
      /**
        * A strategy receives a vector of objects, processors and a solution
        * object to store the solution. Solution has a method `assign(O,P)` to
        * assign an object to a processor. It is meant to be opaque to the
        * strategy. For example, it could directly store the results in a message.
        */
      virtual void solve(std::vector<O> &objs, std::vector<P> &procs, S &solution,
                         bool objsSorted=false) = 0;
      virtual ~Strategy() {}
  };

  template <typename O, typename P, typename S>
  class Random : public Strategy<O,P,S> {
    public:
      Random() {
        std::random_device rd;
        rng = std::mt19937(rd());
      }
      void solve(std::vector<O> &objs, std::vector<P> &procs, S &solution, bool objsSorted) {
        std::uniform_int_distribution<int> uni(0, procs.size() - 1);
        for (const auto &o : objs)
          solution.assign(o, procs[uni(rng)]);
      }
    private:
      std::mt19937 rng;
  };

  template <typename O, typename P, typename S>
  class Dummy : public Strategy<O,P,S> {
    public:
      void solve(std::vector<O> &objs, std::vector<P> &procs, S &solution, bool objsSorted) {}  // do nothing
  };

  template <typename O, typename P, typename S>
  class Rotate : public Strategy<O,P,S> {
    public:
      // TODO we should print a warning message if Rotate is used (it is only
      // useful for diagnostic purposes). However, it probably shouldn't be
      // printed by Rotate because, depending on the tree configuration used with TreeLB,
      // multiple Rotate strategies can be instantiated across PEs or even in the
      // same PE. And it is also possible that Rotates are instantiated but not on PE 0.
      void solve(std::vector<O> &objs, std::vector<P> &procs, S &solution, bool objsSorted) {
        std::sort(procs.begin(), procs.end(), CmpId<P>());
        // could use unordered_map but vector is faster and doesn't require much memory, even
        // for a few million PEs
        // It is important to note that a strategy might not receive all objects
        // and processors, depending on the TreeLB hierarchy used
        std::vector<int> procMap(CkNumPes(), -1);  // real pe -> idx in procs
        for (int i=0; i < procs.size(); i++) procMap[procs[i].id] = i;
        for (const auto &o : objs) {
          if (procMap[o.oldPe] == -1) CkAbort("Rotate does not support foreign objects\n");
          P &p = procs[(procMap[o.oldPe] + 1) % procs.size()];
          solution.assign(o, p);
        }
      }
  };

}


#endif  /* LB_STRATEGY_H */
