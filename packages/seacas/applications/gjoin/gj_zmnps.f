C Copyright(C) 1999-2020 National Technology & Engineering Solutions
C of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with
C NTESS, the U.S. Government retains certain rights in this software.
C
C See packages/seacas/LICENSE for details

C=======================================================================
      SUBROUTINE ZMNPS (NUMNPS, LNPSNL,
     &   IDNPS, NNNPS, IXNNPS, LTNNPS, FACNPS)
C=======================================================================

C   --*** ZMNPS *** (GJOIN) Compress nodal point sets
C   --   Written by Amy Gilkey - revised 01/20/88
C   --
C   --ZMNPS compresses the nodal point sets by removing deleted nodes.
C   --Assumes that the nodes are already renumbered and ordered.
C   --
C   --Parameters:
C   --   NUMNPS - IN - the number of nodal point sets
C   --   LNPSNL - IN/OUT - the length of the nodal point sets node list
C   --   IDNPS - IN - the nodal point set ID for each set
C   --   NNNPS - IN/OUT - the number of nodes for each set
C   --   IXNNPS - IN/OUT - the index of the first node for each set
C   --   LTNNPS - IN/OUT - the nodes for all sets
C   --   FACNPS - IN/OUT - the distribution factors for all sets

      INTEGER IDNPS(*)
      INTEGER NNNPS(*)
      INTEGER IXNNPS(*)
      INTEGER LTNNPS(*)
      REAL FACNPS(*)

      IF (NUMNPS .LE. 0) RETURN

      JNPS = 0
      JNN = 0
      DO 110 INPS = 1, NUMNPS
         JNNLST = JNN
         DO 100 NN = IXNNPS(INPS), IXNNPS(INPS)+NNNPS(INPS)-1
            IF (LTNNPS(NN) .GT. 0) THEN
               JNN = JNN + 1
               LTNNPS(JNN) = LTNNPS(NN)
               FACNPS(JNN) = FACNPS(NN)
            END IF
  100    CONTINUE
         N = JNN - JNNLST
         IF (N .GT. 0) THEN
            JNPS = JNPS + 1
            IDNPS(JNPS) = IDNPS(INPS)
            NNNPS(JNPS) = N
            IXNNPS(JNPS) = JNNLST + 1
         END IF
  110 CONTINUE

      NUMNPS = JNPS
      LNPSNL = JNN

      RETURN
      END
