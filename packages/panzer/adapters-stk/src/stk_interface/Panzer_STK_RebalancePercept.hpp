// @HEADER
// ***********************************************************************
//
//           Panzer: A partial differential equation assembly
//       engine for strongly coupled complex multiphysics systems
//                 Copyright (2011) Sandia Corporation
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
// Questions? Contact Roger P. Pawlowski (rppawlo@sandia.gov) and
// Eric C. Cyr (eccyr@sandia.gov)
// ***********************************************************************
// @HEADER

#ifndef __Panzer_STK_RebalancePercept_hpp__
#define __Panzer_STK_RebalancePercept_hpp__

#include <Teuchos_RCP.hpp>
#include <Teuchos_DefaultMpiComm.hpp>

#include <stk_mesh/base/Types.hpp>
#include <stk_mesh/base/MetaData.hpp>
#include <stk_mesh/base/BulkData.hpp>
#include <stk_mesh/base/Field.hpp>
#include <stk_mesh/base/FieldBase.hpp>
#include <stk_mesh/base/MetaData.hpp>
#include <stk_mesh/base/CoordinateSystems.hpp>

#include "Kokkos_Core.hpp"

#include <Shards_CellTopology.hpp>
#include <Shards_CellTopologyData.h>

#include <PanzerAdaptersSTK_config.hpp>
#include <Kokkos_ViewFactory.hpp>

#include <unordered_map>

#ifdef PANZER_HAVE_IOSS
#include <stk_io/StkMeshIoBroker.hpp>
#endif

#if defined(PANZER_HAVE_STKBALANCE) && defined(PANZER_HAVE_PERCEPT)
#include <stk_balance/balanceUtils.hpp>
#include <percept/PerceptMesh.hpp>

// forward declarations for required percept features
namespace percept {
  class PerceptMesh;
  class URP_Heterogeneous_3D;
}

  //setup_mesh(auraOption);
  //create_child_elements(*m_relationManager);
  //set_element_weights(*m_relationManager);
  //check_initial_parent_child_relation(*m_relationManager);
  //rebalance_parent_elements_with_manager(*m_relationManager);
  //check_expected_element_partitioning();


namespace panzer_stk {
  /** This class derived from stk::balance::BalanceSettings is required in order
    * to utilize stk's mesh balancing utilities. Assuming the number of processors and 
    * elements of the provided mesh are equal, this rebalances the regularly refined 
    * mesh so that each processor obtains one parent element and all its children. It 
    * then makes sure to pair nodes with parents as well.
    */
class Panzer_STK_RebalancePercept : public stk::balance::BalanceSettings
{
public:
  /** Constructor
    * \param[in] selector The selector for the Percept parent elements.
    * \param[in] stkMeshBulkData The STK mesh bulk data
    * \param[in] weightField The weights for the balancing
    * \param[in] perceptMesh The Percept mesh object
   */
  Panzer_STK_RebalancePercept(const stk::mesh::Selector & selector,
                              stk::mesh::BulkData &stkMeshBulkData,
                              const stk::mesh::Field<double> &weightField,
                              Teuchos::RCP<percept::PerceptMesh> perceptMesh)
    : m_stkMeshBulkData(stkMeshBulkData),
      m_weightField(weightField),
      m_parentElementSelector(selector),
      m_perceptMesh(perceptMesh)
    { 
      set_parent_element_weights();
    }

  /** Override the decomposition algorithm to use rcb.
   * The default for stk::balance::BalanceSettings is the TPL ParMETIS,
   * which would certainly break existing package configurations.
   */
  std::string getDecompMethod() const override
  {
    return std::string("rcb");
  }

  /** Modifies the decomposition to move related entities with the parent elements
   */
  virtual void modifyDecomposition(stk::balance::DecompositionChangeList & decomp) const
  {
    delete_child_elements_from_decomposition(decomp);
    move_related_entities_with_parent_element(decomp);
  }

  /** Sets weights for elements
   */
  void set_element_weights()
  {
    //stk::mesh::EntityVector parentElements;
    //stk::mesh::Selector parentElementSelector = (*m_parentPart) & get_meta().locally_owned_part();
    //const bool sortById = true;
    //stk::mesh::get_entities(get_bulk(), stk::topology::ELEM_RANK, parentElementSelector, parentElements, sortById);

    //set_parent_element_weights(parentElements, parentChildManager);
  }

  /** Sets weights for parent elements
   */
  void set_parent_element_weights()
  {
    std::vector<stk::mesh::Entity> parentElements;
    m_stkMeshBulkData.get_entities(stk::topology::ELEM_RANK,m_parentElementSelector,parentElements);
    for (stk::mesh::Entity parentElement : parentElements) {
      // "Not a child" is not the same as "Is a parent" since an element may be both in Percept.
      // This is only true for root elements
      if(!m_perceptMesh->isChildElement(parentElement)) 
      {
        std::cout << "Found root!" << std::endl;
        double * weight = stk::mesh::field_data(m_weightField, parentElement);
        std::cout << weight << std::endl;
      //EXPECT_TRUE(nullptr != weight);

        *weight = 10.0;
      }
      else
      {
        std::cout << "Found intermediate!" << std::endl;
      }
    }
  }

private:

  const stk::mesh::BulkData &m_stkMeshBulkData;
  const stk::mesh::Field<double> &m_weightField;
  const stk::mesh::Selector &m_parentElementSelector;
  const Teuchos::RCP<percept::PerceptMesh> m_perceptMesh;
  //const double m_defaultWeight;

  stk::mesh::EntityVector get_elements_from_selector(stk::mesh::BulkData & stkMeshBulkData, stk::mesh::Selector selector) const
  {
    stk::mesh::EntityVector elements;
    const bool sortById = true;
    stk::mesh::get_entities(stkMeshBulkData, stk::topology::ELEM_RANK, selector, elements, sortById);
    return elements;
  }

  /** Moves related entities with parents
   */
  void move_related_entities_with_parent_element(stk::balance::DecompositionChangeList & decomp) const
  {
    stk::mesh::EntityProcVec changedEntities = decomp.get_all_partition_changes();
    // for (stk::mesh::EntityProc & entityProcs : changedEntities) {
    //   m_parentChildManager.move_related_entities_with_parent_element(decomp, entityProcs.first, entityProcs.second);
    // }
  }

  /** Deletes children from the decomposition
   */
  void delete_child_elements_from_decomposition(stk::balance::DecompositionChangeList & decomp) const
  {
    stk::mesh::Selector localChildElementSelector = (!m_parentElementSelector) & decomp.get_bulk().mesh_meta_data().locally_owned_part();
    stk::mesh::EntityVector childElements = get_elements_from_selector(decomp.get_bulk(), localChildElementSelector);
    for (const auto & childElement : childElements) {
      decomp.delete_entity(childElement);
    }
  }


  /**  These are copied over from the GraphCreationSettings class
   */
  /*
    virtual double getGraphEdgeWeightForSearch() const override;

    virtual double getGraphEdgeWeight(stk::topology element1Topology, stk::topology element2Topology) const override;
    virtual double getGraphVertexWeight(stk::mesh::Entity entity, int criteria_index = 0) const override;

    virtual int getGraphVertexWeight(stk::topology type) const override;

    virtual GraphOption getGraphOption() const override;
    virtual bool includeSearchResultsInGraph() const override;
    virtual void setIncludeSearchResultsInGraph(bool doContactSearch) override;
    virtual double getToleranceForParticleSearch() const override;

    virtual double getToleranceForFaceSearch(const stk::mesh::BulkData & mesh,
                                             const stk::mesh::FieldBase & coordField,
                                             const stk::mesh::Entity * faceNodes,
                                             const unsigned numFaceNodes) const override;
    virtual void setToleranceFunctionForFaceSearch(std::shared_ptr<stk::balance::FaceSearchTolerance> faceSearchTolerance) override;
    virtual bool isConstantFaceSearchTolerance() const override;

    virtual bool getEdgesForParticlesUsingSearch() const override;
    virtual double getVertexWeightMultiplierForVertexInSearch() const override;
    virtual std::string getDecompMethod() const override;

    virtual void setDecompMethod(const std::string& input_method) override;
    virtual void setToleranceForFaceSearch(double tol) override;
    virtual void setToleranceForParticleSearch(double tol);
    virtual void setEdgeWeightForSearch(double w) override;
    virtual void setVertexWeightMultiplierForVertexInSearch(double w) override;
    virtual void setShouldFixSpiders(bool fixSpiders) override;

    virtual bool shouldFixMechanisms() const override;
    virtual bool shouldFixSpiders() const override;
    virtual const stk::mesh::Field<int> * getSpiderBeamConnectivityCountField(const stk::mesh::BulkData & stkMeshBulkData) const override;
    virtual const stk::mesh::Field<int> * getSpiderVolumeConnectivityCountField(const stk::mesh::BulkData & stkMeshBulkData) const override;

    virtual void setUseNodeBalancer(bool useBalancer);
    virtual void setNodeBalancerTargetLoadBalance(double targetLoadBalance);
    virtual void setNodeBalancerMaxIterations(unsigned maxIterations);

    virtual bool useNodeBalancer() const override;
    virtual double getNodeBalancerTargetLoadBalance() const override;
    virtual unsigned getNodeBalancerMaxIterations() const override;
    */
};







} // end namespace panzer_stk

#endif // end if PANZER_HAVE_STKBALANCE and PANZER_HAVE_PERCEPT

#endif
