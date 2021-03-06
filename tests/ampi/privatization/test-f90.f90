      !!! Fortran module variables, implicit and explicit save variables,
      !!! and common blocks are all unsafe if mutable, and must be privatized.
      !!! Parameter variables are immutable and so need not be privatized.

      module test_mod

        implicit none

        integer, parameter :: parameter_variable = 0
        integer :: module_variable

      end module test_mod


      subroutine mpi_main

        implicit none
        include 'mpif.h'

        integer :: ierr

        call mpi_init(ierr)

        call privatization_test_framework()

        call mpi_finalize(ierr)

      end subroutine mpi_main


      subroutine perform_test_batch(failed, rank, my_wth)

        use test_mod
        implicit none

        integer :: failed, rank, my_wth
        integer :: save_variable1 = 0
        integer, save :: save_variable2
        integer :: common_variable
        common /commons/ common_variable

        call test_privatization(failed, rank, my_wth, module_variable)
        call test_privatization(failed, rank, my_wth, save_variable1)
        call test_privatization(failed, rank, my_wth, save_variable2)
        call test_privatization(failed, rank, my_wth, common_variable)

      end subroutine perform_test_batch
