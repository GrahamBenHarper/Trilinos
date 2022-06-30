// @HEADER
//
// ***********************************************************************
//
//        MueLu: A package for multigrid based preconditioning
//                  Copyright 2012 Sandia Corporation
//
// Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
// the U.S. Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact
//                    Jonathan Hu       (jhu@sandia.gov)
//                    Andrey Prokopenko (aprokop@sandia.gov)
//                    Ray Tuminaro      (rstumin@sandia.gov)
//
// ***********************************************************************
//
// @HEADER
#ifndef MUELU_CONSTRAINTFACTORY_DEF_HPP
#define MUELU_CONSTRAINTFACTORY_DEF_HPP

#include <Xpetra_MultiVector_fwd.hpp>
#include <Xpetra_CrsGraph_fwd.hpp>

#include "MueLu_ConstraintFactory_decl.hpp"
#include "MueLu_FactoryManagerBase.hpp"

#include "MueLu_Constraint.hpp"
#include "MueLu_Monitor.hpp"

namespace MueLu {

  template <class Scalar, class LocalOrdinal, class GlobalOrdinal, class Node>
  RCP<const ParameterList> ConstraintFactory<Scalar, LocalOrdinal, GlobalOrdinal, Node>::GetValidParameterList() const {
    RCP<ParameterList> validParamList = rcp(new ParameterList());

    validParamList->set< RCP<const FactoryBase> >("FineNullspace",    Teuchos::null, "Generating factory for the nullspace");
    validParamList->set< RCP<const FactoryBase> >("CoarseNullspace",  Teuchos::null, "Generating factory for the nullspace");
    validParamList->set< RCP<const FactoryBase> >("Ppattern",         Teuchos::null, "Generating factory for the nonzero pattern");

    return validParamList;
  }

  template <class Scalar, class LocalOrdinal, class GlobalOrdinal, class Node>
  void ConstraintFactory<Scalar, LocalOrdinal, GlobalOrdinal, Node>::DeclareInput(Level &fineLevel, Level& coarseLevel) const {
    Input(fineLevel,   "Nullspace", "FineNullspace");
    Input(coarseLevel, "Nullspace", "CoarseNullspace");
    Input(coarseLevel, "Ppattern");
    
    if(coarseLevel.IsAvailable("AdditionalConstraints", NoFactory::get()))
      coarseLevel.DeclareInput("AdditionalConstraints", NoFactory::get(), this);
  }

  template <class Scalar, class LocalOrdinal, class GlobalOrdinal, class Node>
  void ConstraintFactory<Scalar, LocalOrdinal, GlobalOrdinal, Node>::Build(Level &fineLevel, Level& coarseLevel) const {
    FactoryMonitor m(*this, "Constraint", coarseLevel);
    std::cout << "Build ConstraintFactory" << std::endl;
    RCP<MultiVector> fineNullspace   = Get< RCP<MultiVector> >(fineLevel,   "Nullspace", "FineNullspace");
    RCP<MultiVector> coarseNullspace = Get< RCP<MultiVector> >(coarseLevel, "Nullspace", "CoarseNullspace");
    std::cout << "Got nullspaces" << std::endl;
    // Fetch the additional constraints of a level and count them if there are any
    size_t numNullspace = coarseNullspace->getNumVectors();
    size_t numAdditionalConstraints = 0;
    RCP<MultiVector> additionalConstraints;
    if(coarseLevel.IsAvailable("AdditionalConstraints", NoFactory::get())) {
      std::cout << "Found additional constraints on the level!" << std::endl;
      additionalConstraints = Get< RCP<MultiVector> >(coarseLevel, "AdditionalConstraints");
      if(!additionalConstraints.is_null()) {
        std::cout << "Found a nonzero number of additional constraints on the level!" << std::endl;
        numAdditionalConstraints = additionalConstraints->getNumVectors();
      }
    }

    if(fineNullspace.is_null())
      std::cout << "Nullptr fine nullspace " << std::endl;
    if(coarseNullspace.is_null())
      std::cout << "Nullptr coarse nullspace " << std::endl;
    std::cout << "Building constraints... detected " << numAdditionalConstraints << " additional" << std::endl;
    RCP<Constraint> constraint(new Constraint);
    // Do things the usual way if there are no additional constraints
    if(numAdditionalConstraints == 0) {
      std::cout << "Building constraints without additional" << std::endl;
      constraint->Setup(*fineNullspace, *coarseNullspace,
                        Get< RCP<const CrsGraph> >(coarseLevel, "Ppattern"));

    } else { // Otherwise create a new MV where we smash the nullspace and constraints all together
      std::cout << "Building constraints with additional" << std::endl;
      std::vector<Teuchos::ArrayView<const SC> > coarseNullspaceColVals;
      for(size_t i = 0; i < numNullspace; ++i) {
        Teuchos::ArrayRCP<const SC> coarseColRCP = coarseNullspace->getData(i);
        coarseNullspaceColVals.push_back(coarseColRCP());
      }

      Teuchos::ArrayView<Teuchos::ArrayView<const SC> > coarseNullspaceVals = coarseNullspaceColVals;
      Teuchos::ArrayView<Teuchos::ArrayView<const SC> > addionalConstraintVals;
      size_t numConstraints = numNullspace + numAdditionalConstraints;

      RCP<const Map> map = coarseNullspace->getMap();
      RCP<MultiVector> constraintMV = Xpetra::MultiVectorFactory<Scalar,LocalOrdinal,GlobalOrdinal,Node>::Build(map, coarseNullspaceVals, numConstraints);

      constraint->Setup(*fineNullspace, *constraintMV,
                        Get< RCP<const CrsGraph> >(coarseLevel, "Ppattern"));
    }
    
    Set(coarseLevel, "Constraint", constraint);
  }


} // namespace MueLu

#endif // MUELU_CONSTRAINTFACTORY_DEF_HPP
