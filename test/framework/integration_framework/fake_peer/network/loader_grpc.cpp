/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "framework/integration_framework/fake_peer/network/loader_grpc.hpp"

#include "backend/protobuf/block.hpp"
#include "framework/integration_framework/fake_peer/behaviour/behaviour.hpp"
#include "framework/integration_framework/fake_peer/fake_peer.hpp"
#include "loader.grpc.pb.h"
#include "network/impl/grpc_channel_builder.hpp"

namespace integration_framework {
  namespace fake_peer {

    LoaderGrpc::LoaderGrpc(const std::shared_ptr<FakePeer> &fake_peer)
        : fake_peer_wptr_(fake_peer) {
      log_ = logger::log(
          "IntegrationTestFramework (fake peer at "
          + fake_peer->getAddress() + " Loader transport)");
        }

    ::grpc::Status LoaderGrpc::retrieveBlock(
        ::grpc::ServerContext *context,
        const iroha::network::proto::BlockRequest *request,
        iroha::protocol::Block *response) {
      LoaderBlockRequest hash =
          std::make_shared<shared_model::crypto::Hash>(request->hash());
      auto fake_peer = fake_peer_wptr_.lock();
      BOOST_VERIFY_MSG(fake_peer, "Fake Peer is not set!");
      auto behaviour = fake_peer->getBehaviour();
      if (!behaviour) {
        return ::grpc::Status(::grpc::StatusCode::INTERNAL,
                              "Fake Peer has no behaviour set!");
      }
      auto block = behaviour->processLoaderBlockRequest(hash);
      if (!block) {
        return ::grpc::Status(::grpc::StatusCode::NOT_FOUND, "Block not found");
      }
      *response = block->getTransport();
      return ::grpc::Status::OK;
    }

    ::grpc::Status LoaderGrpc::retrieveBlocks(
        ::grpc::ServerContext *context,
        const iroha::network::proto::BlocksRequest *request,
        ::grpc::ServerWriter<iroha::protocol::Block> *writer) {
      LoaderBlocksRequest height = request->height();
      auto fake_peer = fake_peer_wptr_.lock();
      BOOST_VERIFY_MSG(fake_peer, "Fake peer is not set!");
      auto behaviour = fake_peer->getBehaviour();
      if (!behaviour) {
        return ::grpc::Status(::grpc::StatusCode::INTERNAL,
                              "Fake Peer has no behaviour set!");
      }
      auto blocks = behaviour->processLoaderBlocksRequest(height);
      for (auto &block : blocks) {
        writer->Write(block.get().getTransport());
      }
      return ::grpc::Status::OK;
    }

    bool LoaderGrpc::sendBlockRequest(
        const std::string &dest_address, const LoaderBlockRequest &hash) {
      iroha::network::proto::BlockRequest request;
      request.set_hash(hash->hex());
      grpc::ClientContext context;
      iroha::protocol::Block block;
      auto client = iroha::network::createClient<iroha::network::proto::Loader>(
          dest_address);

      const auto status = client->retrieveBlock(&context, request, &block);
      if (not status.ok()) {
        log_->warn("Error retrieving block: " + status.error_message());
        return false;
      }
      return true;
    }

    size_t LoaderGrpc::sendBlocksRequest(const std::string &dest_address,
                                         const LoaderBlocksRequest &height) {
      iroha::network::proto::BlocksRequest request;
      request.set_height(height);
      grpc::ClientContext context;
      iroha::protocol::Block block;
      auto client = iroha::network::createClient<iroha::network::proto::Loader>(
          dest_address);

      auto reader = client->retrieveBlocks(&context, request);
      size_t num_read_blocks = 0;
      while (reader->Read(&block)) {
        ++num_read_blocks;
      }

      return num_read_blocks;
    }

  }  // namespace fake_peer
}  // namespace integration_framework
