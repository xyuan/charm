#include "migration.h"

void migration_init(void)
{
  //const int numElements = 10 + (CkNumPes() * 2);
  // Having numElements as CkNumPes() is a smaller condition to reproduce the hang
  const int numElements = CkNumPes();
  if(CkNumPes() < 2) {
    CkError("migration: requires at least 2 processors.\n");
    megatest_finish();
  } else {
    CmiPrintf("[%d][%d][%d] migration_init: Creating 1d array proxy with numElements: %d\n", CmiMyPe(), CmiMyNode(), CmiMyRank(), numElements);
    CProxy_mig_Element::ckNew(numElements);
  }
}

void migration_moduleinit(void){}

mig_Element::mig_Element()
{
  origPE = -1;
  sum = 0;
  index = thisIndex;
  numDone = 0;
  CmiPrintf("[%d][%d][%d][%d] mig_Element::mig_Element constructor\n", CmiMyPe(), CmiMyNode(), CmiMyRank(), thisIndex);
  CProxy_mig_Element self(thisArrayID);
  self[thisIndex].arrive();
}

void mig_Element::pup(PUP::er &p)
{
  p(origPE);
  p(sum);
  p(numDone);
  p(index);
}

void 
mig_Element::arrive(void)
{
  CmiPrintf("[%d][%d][%d][%d] mig_Element::arrive beg\n", CmiMyPe(), CmiMyNode(), CmiMyRank(), thisIndex);

  if (thisIndex != index) 
    CkAbort("migration: chare-data corrupted!\n");
  CProxy_mig_Element self(thisArrayID);
  if(CkMyPe() == origPE) {
    if(sum != ((CkNumPes()+1)*CkNumPes())/2)
      CkAbort("migrate: Element did not migrate to all the processors!\n");
    CmiPrintf("[%d][%d][%d][%d] mig_Element::arrive calling done\n", CmiMyPe(), CmiMyNode(), CmiMyRank(), thisIndex);
    self[0].done();
  } else {
    if(origPE==(-1)) origPE = CkMyPe();
    sum += CkMyPe() + 1;
    int nextPE = (CkMyPe()+1)%CkNumPes();
    CmiPrintf("[%d][%d][%d][%d] mig_Element::migrateMe to %d\n", CmiMyPe(), CmiMyNode(), CmiMyRank(), thisIndex, nextPE);
    migrateMe(nextPE);
  }  
}

void
mig_Element::ckJustMigrated()
{
  CmiPrintf("[%d][%d][%d][%d] mig_Element::ckJustMigrated\n", CmiMyPe(), CmiMyNode(), CmiMyRank(), thisIndex);
  CProxy_mig_Element self(thisArrayID);
  self[thisIndex].arrive();
}

void
mig_Element::done(void)
{
  numDone++;
  CmiPrintf("[%d][%d][%d] mig_Element::done and numDone is %d and ckGetArraySize is %d\n", CmiMyPe(), CmiMyNode(), CmiMyRank(), numDone, ckGetArraySize());
  if(numDone==ckGetArraySize()) {
    CmiPrintf("[%d][%d][%d][%d] mig_Element::done and calling megatest_finish\n", CmiMyPe(), CmiMyNode(), CmiMyRank(), thisIndex);
    megatest_finish();
  }
}

MEGATEST_REGISTER_TEST(migration,"jackie",1)
#include "migration.def.h"
