/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef INTEGRATION_FRAMEWORK_FAKE_PEER_BEHAVIOUR_DELAYED_HPP_
#define INTEGRATION_FRAMEWORK_FAKE_PEER_BEHAVIOUR_DELAYED_HPP_

#include <chrono>

#include "framework/integration_framework/fake_peer/behaviour/decorator.hpp"

namespace integration_framework {
  namespace fake_peer {

    class DelayedBehaviour : public BehaviourDecorator {
     public:
      DelayedBehaviour(std::unique_ptr<Behaviour> &&base_behaviour,
                       std::chrono::milliseconds delay);

      virtual ~DelayedBehaviour() = default;

      void processMstMessage(MstMessagePtr message) override;
      void processYacMessage(YacMessagePtr message) override;
      void processOsBatch(OsBatchPtr batch) override;
      void processOgProposal(OgProposalPtr proposal) override;
      LoaderBlockRequestResult processLoaderBlockRequest(
          LoaderBlockRequest request) override;
      LoaderBlocksRequestResult processLoaderBlocksRequest(
          LoaderBlocksRequest request) override;

      std::string getName() override;

     private:
      std::chrono::milliseconds delay_;
    };

  }  // namespace fake_peer
}  // namespace integration_framework

#endif /* INTEGRATION_FRAMEWORK_FAKE_PEER_BEHAVIOUR_DELAYED_HPP_ */
