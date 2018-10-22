/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef FAKE_PEER_YAC_NETWORK_NOTIFIER_HPP_
#define FAKE_PEER_YAC_NETWORK_NOTIFIER_HPP_

#include <rxcpp/rx.hpp>

#include "consensus/yac/transport/yac_network_interface.hpp"

namespace iroha {
  namespace consensus {
    namespace yac {
      class NetworkImpl;
      class VoteMessage;
    }  // namespace yac
  }  // namespace consensus
}  // namespace iroha

namespace integration_framework {

  class YacNetworkNotifier final
      : public iroha::consensus::yac::YacNetworkNotifications,
        public std::enable_shared_from_this<YacNetworkNotifier> {
   public:
    using StateMessage = std::vector<iroha::consensus::yac::VoteMessage>;
    using StateMessagePtr = std::shared_ptr<const StateMessage>;

    void subscribe(const std::shared_ptr<iroha::consensus::yac::NetworkImpl>
                       &yac_transport);

    void onState(StateMessage state) override;

    rxcpp::observable<StateMessagePtr> get_observable();

   private:
    rxcpp::subjects::subject<StateMessagePtr> votes_subject_;
  };

}  // namespace integration_framework

#endif /* FAKE_PEER_YAC_NETWORK_NOTIFIER_HPP_ */
