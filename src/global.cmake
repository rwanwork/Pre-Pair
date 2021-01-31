############################################################
##  CMake file defining global variables
##
##  $LastChangedDate: 2014-06-10 01:14:20 +0800 (Tue, 10 Jun 2014) $
##  $LastChangedRevision: 17 $
##
############################################################

##  Software versions
SET (GLOBAL_VERSION_MAJOR 1)
SET (GLOBAL_VERSION_MINOR 2)

SET (REPAIR_VERSION_MAJOR "${GLOBAL_VERSION_MAJOR}")
SET (REPAIR_VERSION_MINOR "${GLOBAL_VERSION_MINOR}")

SET (DESPAIR_VERSION_MAJOR "${GLOBAL_VERSION_MAJOR}")
SET (DESPAIR_VERSION_MINOR "${GLOBAL_VERSION_MINOR}")

SET (REMERGE_VERSION_MAJOR "${GLOBAL_VERSION_MAJOR}")
SET (REMERGE_VERSION_MINOR "${GLOBAL_VERSION_MINOR}")

SET (PREPAIR_VERSION_MAJOR "${GLOBAL_VERSION_MAJOR}")
SET (PREPAIR_VERSION_MINOR "${GLOBAL_VERSION_MINOR}")


##  Testing compilation flags, some of which are suggested by the Valgrind 3.3 book
# SET (MY_C_FLAGS "-pedantic -Wno-long-long -g -fno-inline -O0 -Wall")
##  Release compilation flags, suggested by the Valgrind 3.3 book
SET (MY_C_FLAGS "-O3 -Wall")

