//@HEADER
// ************************************************************************
//
//               ShyLU: Hybrid preconditioner package
//                 Copyright 2012 Sandia Corporation
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
// Questions? Contact Alexander Heinlein (alexander.heinlein@uni-koeln.de)
//
// ************************************************************************
//@HEADER

#ifndef _STRATIMIKOS_FROSCH_DEF_HPP
#define _STRATIMIKOS_FROSCH_DEF_HPP

#include "Stratimikos_FROSch_decl.hpp"

#include "Thyra_FROSchFactory_def.hpp"


namespace Stratimikos {

  using namespace std;
  using namespace Teuchos;
  using namespace Thyra;

  template <typename LO,typename GO,typename NO>
  void enableFROSch (LinearSolverBuilder<double>& builder,
                     const string& stratName)
  {
    const RCP<const ParameterList> precValidParams = sublist(builder.getValidParameters(), "Preconditioner Types");

    TEUCHOS_TEST_FOR_EXCEPTION(precValidParams->isParameter(stratName), logic_error,
                               "Stratimikos::enableFROSch cannot add \"" + stratName +"\" because it is already included in builder!");

    using Base = PreconditionerFactoryBase<double>;
    if (!stratName.compare("FROSch")) {
      using Impl = FROSchFactory<double, LO, GO, NO>;
      builder.setPreconditioningStrategyFactory(abstractFactoryStd<Base, Impl>(), stratName);
    }
  }

  template <typename SC, typename LO,typename GO,typename NO>
  void enableFROSch (LinearSolverBuilder<SC>& builder,
                     const string& stratName)
  {
    const RCP<const ParameterList> precValidParams = sublist(builder.getValidParameters(), "Preconditioner Types");

    TEUCHOS_TEST_FOR_EXCEPTION(precValidParams->isParameter(stratName), logic_error,
                               "Stratimikos::enableFROSch cannot add \"" + stratName +"\" because it is already included in builder!");

    using Base = PreconditionerFactoryBase<SC>;
    if (!stratName.compare("FROSch")) {
      using Impl = FROSchFactory<SC, LO, GO, NO>;
      builder.setPreconditioningStrategyFactory(abstractFactoryStd<Base, Impl>(), stratName);
    }
  }

} // namespace Stratimikos

#endif
