// Copyright (c) Improbable Worlds Ltd, All Rights Reserved
#ifndef WORKER_SDK_CPP_INCLUDE_IMPROBABLE_DETAIL_DISPATCHER_I_H
#define WORKER_SDK_CPP_INCLUDE_IMPROBABLE_DETAIL_DISPATCHER_I_H
#include <improbable/detail/client_handle.i.h>
#include <improbable/worker.h>

namespace worker {
namespace detail {
inline void HandleDisconnect(const detail::DispatcherImpl& impl,
                             const internal::WorkerProtocol_DisconnectOp& op) {
  DisconnectOp wrapper{op.Reason};
  impl.disconnect_callbacks.InvokeAll(wrapper);
}

inline void HandleFlagUpdate(const detail::DispatcherImpl& impl,
                             const internal::WorkerProtocol_FlagUpdateOp& op) {
  FlagUpdateOp wrapper{op.Name, {}};
  if (op.Value) {
    wrapper.Value.emplace(op.Value);
    impl.flag_update_callbacks.InvokeAll(wrapper);
  } else {
    impl.flag_update_callbacks.ReverseInvokeAll(wrapper);
  }
}

inline void HandleLogMessage(const detail::DispatcherImpl& impl,
                             const internal::WorkerProtocol_LogMessageOp& op) {
  LogMessageOp wrapper{static_cast<LogLevel>(op.LogLevel), op.Message};
  impl.log_message_callbacks.InvokeAll(wrapper);
}

inline void HandleMetrics(const detail::DispatcherImpl& impl,
                          const internal::WorkerProtocol_MetricsOp& op) {
  MetricsOp wrapper;
  for (std::size_t i = 0; i < op.Metrics.GaugeMetricCount; ++i) {
    wrapper.Metrics.GaugeMetrics[op.Metrics.GaugeMetric[i].Key] = op.Metrics.GaugeMetric[i].Value;
  }
  // We do not have any built-in histogram metrics.
  impl.metrics_callbacks.InvokeAll(wrapper);
}

inline void HandleCriticalSection(const detail::DispatcherImpl& impl,
                                  const internal::WorkerProtocol_CriticalSectionOp& op) {
  CriticalSectionOp wrapper{op.InCriticalSection != 0};
  if (op.InCriticalSection) {
    impl.critical_section_callbacks.InvokeAll(wrapper);
  } else {
    impl.critical_section_callbacks.ReverseInvokeAll(wrapper);
  }
}

inline void HandleAddEntity(const detail::DispatcherImpl& impl,
                            const internal::WorkerProtocol_AddEntityOp& op) {
  AddEntityOp wrapper{op.EntityId};
  impl.add_entity_callbacks.InvokeAll(wrapper);
}

inline void HandleRemoveEntity(const detail::DispatcherImpl& impl,
                               const internal::WorkerProtocol_RemoveEntityOp& op) {
  RemoveEntityOp wrapper{op.EntityId};
  impl.remove_entity_callbacks.ReverseInvokeAll(wrapper);
}

inline void
HandleReserveEntityIdResponse(const detail::DispatcherImpl& impl,
                              const internal::WorkerProtocol_ReserveEntityIdResponseOp& op) {
  ReserveEntityIdResponseOp wrapper{RequestId<ReserveEntityIdRequest>{op.RequestId},
                                    static_cast<worker::StatusCode>(op.StatusCode), op.Message,
                                    op.StatusCode == internal::WORKER_PROTOCOL_STATUS_CODE_SUCCESS
                                        ? op.EntityId
                                        : Option<EntityId>{}};
  impl.reserve_entity_id_response_callbacks.InvokeAll(wrapper);
}

inline void
HandleReserveEntityIdsResponse(const detail::DispatcherImpl& impl,
                               const internal::WorkerProtocol_ReserveEntityIdsResponseOp& op) {
  ReserveEntityIdsResponseOp wrapper{RequestId<ReserveEntityIdsRequest>{op.RequestId},
                                     static_cast<worker::StatusCode>(op.StatusCode), op.Message,
                                     op.StatusCode == internal::WORKER_PROTOCOL_STATUS_CODE_SUCCESS
                                         ? op.FirstEntityId
                                         : Option<EntityId>{},
                                     op.NumberOfEntityIds};
  impl.reserve_entity_ids_response_callbacks.InvokeAll(wrapper);
}

inline void HandleCreateEntityResponse(const detail::DispatcherImpl& impl,
                                       const internal::WorkerProtocol_CreateEntityResponseOp& op) {
  CreateEntityResponseOp wrapper{RequestId<CreateEntityRequest>{op.RequestId},
                                 static_cast<worker::StatusCode>(op.StatusCode), op.Message,
                                 op.StatusCode == internal::WORKER_PROTOCOL_STATUS_CODE_SUCCESS
                                     ? op.EntityId
                                     : Option<EntityId>{}};
  impl.create_entity_response_callbacks.InvokeAll(wrapper);
}

inline void HandleDeleteEntityResponse(const detail::DispatcherImpl& impl,
                                       const internal::WorkerProtocol_DeleteEntityResponseOp& op) {
  DeleteEntityResponseOp wrapper{RequestId<DeleteEntityRequest>{op.RequestId}, op.EntityId,
                                 static_cast<worker::StatusCode>(op.StatusCode), op.Message};
  impl.delete_entity_response_callbacks.InvokeAll(wrapper);
}

inline void HandleEntityQueryResponse(const detail::DispatcherImpl& impl,
                                      const internal::WorkerProtocol_EntityQueryResponseOp& op) {
  EntityQueryResponseOp wrapper{RequestId<EntityQueryRequest>{op.RequestId},
                                static_cast<worker::StatusCode>(op.StatusCode),
                                op.Message,
                                op.ResultCount,
                                {}};
  for (std::uint32_t i = 0; op.Result && i < op.ResultCount; ++i) {
    auto& entity = wrapper.Result[op.Result[i].EntityId];
    for (std::uint32_t j = 0; j < op.Result[i].ComponentCount; ++j) {
      const auto& component = op.Result[i].Component[j];
      auto it = impl.component_info.MoveSnapshotIntoEntity.find(component.ComponentId);
      if (it != impl.component_info.MoveSnapshotIntoEntity.end()) {
        // Moves the snapshot data out of the C-owned object. It's OK to steal the data here since
        // we know this is the only possible callback for the C dispatcher.
        it->second(const_cast<detail::ClientHandleBase*>(
                       static_cast<const detail::ClientHandleBase*>(component.Handle)),
                   entity);
      }
    }
  }
  impl.entity_query_response_callbacks.InvokeAll(wrapper);
}

inline void HandleAddComponent(const detail::DispatcherImpl& impl,
                               const internal::WorkerProtocol_AddComponentOp& op) {
  detail::DispatcherImpl::ComponentWrapperOp wrapper{
      op.EntityId, static_cast<const detail::ClientHandleBase*>(op.InitialComponent.Handle)};
  impl.add_component_callbacks.InvokeAll(op.InitialComponent.ComponentId, wrapper);
}

inline void HandleRemoveComponent(const detail::DispatcherImpl& impl,
                                  const internal::WorkerProtocol_RemoveComponentOp& op) {
  RemoveComponentOp wrapper{op.EntityId};
  impl.remove_component_callbacks.ReverseInvokeAll(op.ComponentId, wrapper);
}

inline void HandleAuthorityChange(const detail::DispatcherImpl& impl,
                                  const internal::WorkerProtocol_AuthorityChangeOp& op) {
  AuthorityChangeOp wrapper{op.EntityId, static_cast<worker::Authority>(op.Authority)};
  if (wrapper.Authority == Authority::kAuthoritative) {
    impl.authority_change_callbacks.InvokeAll(op.ComponentId, wrapper);
  } else {
    impl.authority_change_callbacks.ReverseInvokeAll(op.ComponentId, wrapper);
  }
}

inline void HandleComponentUpdate(const detail::DispatcherImpl& impl,
                                  const internal::WorkerProtocol_ComponentUpdateOp& op) {
  detail::DispatcherImpl::ComponentWrapperOp wrapper{
      op.EntityId, static_cast<const detail::ClientHandleBase*>(op.Update.Handle)};
  impl.component_update_callbacks.InvokeAll(op.Update.ComponentId, wrapper);
}

inline void HandleCommandRequest(const detail::DispatcherImpl& impl,
                                 const internal::WorkerProtocol_CommandRequestOp& op) {
  detail::DispatcherImpl::CommandRequestWrapperOp wrapper{
      op.RequestId,
      op.EntityId,
      op.TimeoutMillis,
      op.CallerWorkerId,
      op.CallerAttributes.AttributeCount,
      op.CallerAttributes.Attribute,
      static_cast<const detail::ClientHandleBase*>(op.Request.Handle)};
  impl.command_request_callbacks.InvokeAll(op.Request.ComponentId, wrapper);
}

inline void HandleCommandResponse(const detail::DispatcherImpl& impl,
                                  const internal::WorkerProtocol_CommandResponseOp& op) {
  detail::DispatcherImpl::CommandResponseWrapperOp wrapper{
      op.RequestId,
      op.EntityId,
      op.StatusCode,
      op.Message,
      static_cast<const detail::ClientHandleBase*>(op.Response.Handle),
      op.CommandId};
  impl.command_response_callbacks.InvokeAll(op.Response.ComponentId, wrapper);
}

}  // ::detail

// Implementation of Dispatcher.
inline Dispatcher::Dispatcher(const ComponentRegistry& registry)
: impl{registry.GetInternalComponentInfo()} {}

inline Dispatcher::CallbackKey Dispatcher::OnDisconnect(const Callback<DisconnectOp>& callback) {
  impl.disconnect_callbacks.Add(impl.current_callback_key, callback);
  return impl.current_callback_key++;
}

inline Dispatcher::CallbackKey Dispatcher::OnFlagUpdate(const Callback<FlagUpdateOp>& callback) {
  impl.flag_update_callbacks.Add(impl.current_callback_key, callback);
  return impl.current_callback_key++;
}

inline Dispatcher::CallbackKey Dispatcher::OnLogMessage(const Callback<LogMessageOp>& callback) {
  impl.log_message_callbacks.Add(impl.current_callback_key, callback);
  return impl.current_callback_key++;
}

inline Dispatcher::CallbackKey Dispatcher::OnMetrics(const Callback<MetricsOp>& callback) {
  impl.metrics_callbacks.Add(impl.current_callback_key, callback);
  return impl.current_callback_key++;
}

inline Dispatcher::CallbackKey
Dispatcher::OnCriticalSection(const Callback<CriticalSectionOp>& callback) {
  impl.critical_section_callbacks.Add(impl.current_callback_key, callback);
  return impl.current_callback_key++;
}

inline Dispatcher::CallbackKey Dispatcher::OnAddEntity(const Callback<AddEntityOp>& callback) {
  impl.add_entity_callbacks.Add(impl.current_callback_key, callback);
  return impl.current_callback_key++;
}

inline Dispatcher::CallbackKey
Dispatcher::OnRemoveEntity(const Callback<RemoveEntityOp>& callback) {
  impl.remove_entity_callbacks.Add(impl.current_callback_key, callback);
  return impl.current_callback_key++;
}

inline Dispatcher::CallbackKey
Dispatcher::OnReserveEntityIdResponse(const Callback<ReserveEntityIdResponseOp>& callback) {
  impl.reserve_entity_id_response_callbacks.Add(impl.current_callback_key, callback);
  return impl.current_callback_key++;
}

inline Dispatcher::CallbackKey
Dispatcher::OnReserveEntityIdsResponse(const Callback<ReserveEntityIdsResponseOp>& callback) {
  impl.reserve_entity_ids_response_callbacks.Add(impl.current_callback_key, callback);
  return impl.current_callback_key++;
}

inline Dispatcher::CallbackKey
Dispatcher::OnCreateEntityResponse(const Callback<CreateEntityResponseOp>& callback) {
  impl.create_entity_response_callbacks.Add(impl.current_callback_key, callback);
  return impl.current_callback_key++;
}

inline Dispatcher::CallbackKey
Dispatcher::OnDeleteEntityResponse(const Callback<DeleteEntityResponseOp>& callback) {
  impl.delete_entity_response_callbacks.Add(impl.current_callback_key, callback);
  return impl.current_callback_key++;
}

inline Dispatcher::CallbackKey
Dispatcher::OnEntityQueryResponse(const Callback<EntityQueryResponseOp>& callback) {
  impl.entity_query_response_callbacks.Add(impl.current_callback_key, callback);
  return impl.current_callback_key++;
}

template <typename T>
Dispatcher::CallbackKey Dispatcher::OnAddComponent(const Callback<AddComponentOp<T>>& callback) {
  auto wrapper_callback = [callback](const detail::DispatcherImpl::ComponentWrapperOp& op) {
    callback(
        AddComponentOp<T>{op.EntityId, detail::ClientHandle<typename T::Data>::get(op.Handle)});
  };
  impl.add_component_callbacks.Add(T::ComponentId, impl.current_callback_key, wrapper_callback);
  return impl.current_callback_key++;
}

template <typename T>
Dispatcher::CallbackKey Dispatcher::OnRemoveComponent(const Callback<RemoveComponentOp>& callback) {
  impl.remove_component_callbacks.Add(T::ComponentId, impl.current_callback_key, callback);
  return impl.current_callback_key++;
}

template <typename T>
Dispatcher::CallbackKey Dispatcher::OnAuthorityChange(const Callback<AuthorityChangeOp>& callback) {
  impl.authority_change_callbacks.Add(T::ComponentId, impl.current_callback_key, callback);
  return impl.current_callback_key++;
}

template <typename T>
Dispatcher::CallbackKey
Dispatcher::OnComponentUpdate(const Callback<ComponentUpdateOp<T>>& callback) {
  auto wrapper_callback = [callback](const detail::DispatcherImpl::ComponentWrapperOp& op) {
    callback(ComponentUpdateOp<T>{op.EntityId,
                                  detail::ClientHandle<typename T::Update>::get(op.Handle)});
  };
  impl.component_update_callbacks.Add(T::ComponentId, impl.current_callback_key, wrapper_callback);
  return impl.current_callback_key++;
}

template <typename T>
Dispatcher::CallbackKey
Dispatcher::OnCommandRequest(const Callback<CommandRequestOp<T>>& callback) {
  auto wrapper_callback = [callback](const detail::DispatcherImpl::CommandRequestWrapperOp& op) {
    const auto& generic_request =
        detail::ClientHandle<typename T::ComponentMetaclass::GenericCommandObject>::get(op.Request);
    if (generic_request.CommandId != T::CommandId) {
      return;
    }
    CommandRequestOp<T> wrapper{
        RequestId<IncomingCommandRequest<T>>{op.RequestId},
        op.EntityId,
        op.TimeoutMillis,
        op.CallerWorkerId,
        /* CallerAttributeSet */ {},
        *generic_request.CommandObject.template data<typename T::Request>()};
    for (std::uint32_t i = 0; i < op.CallerAttributeCount; ++i) {
      wrapper.CallerAttributeSet.emplace_back(op.CallerAttribute[i]);
    }
    callback(wrapper);
  };
  impl.command_request_callbacks.Add(T::ComponentMetaclass::ComponentId, impl.current_callback_key,
                                     wrapper_callback);
  return impl.current_callback_key++;
}

template <typename T>
Dispatcher::CallbackKey
Dispatcher::OnCommandResponse(const Callback<CommandResponseOp<T>>& callback) {
  auto wrapper_callback = [callback](const detail::DispatcherImpl::CommandResponseWrapperOp& op) {
    if (op.CommandId != T::CommandId) {
      return;
    }
    CommandResponseOp<T> wrapper{RequestId<OutgoingCommandRequest<T>>{op.RequestId},
                                 op.EntityId,
                                 static_cast<StatusCode>(op.StatusCode),
                                 op.Message,
                                 {}};
    if (op.Response) {
      const auto& generic_response =
          detail::ClientHandle<typename T::ComponentMetaclass::GenericCommandObject>::get(
              op.Response);
      wrapper.Response.emplace(
          *generic_response.CommandObject.template data<typename T::Response>());
    }
    callback(wrapper);
  };
  impl.command_response_callbacks.Add(T::ComponentMetaclass::ComponentId, impl.current_callback_key,
                                      wrapper_callback);
  return impl.current_callback_key++;
}

inline void Dispatcher::Remove(CallbackKey key) {
  if (!impl.disconnect_callbacks.Remove(key) && !impl.flag_update_callbacks.Remove(key) &&
      !impl.log_message_callbacks.Remove(key) && !impl.metrics_callbacks.Remove(key) &&
      !impl.critical_section_callbacks.Remove(key) && !impl.add_entity_callbacks.Remove(key) &&
      !impl.remove_entity_callbacks.Remove(key) &&
      !impl.reserve_entity_id_response_callbacks.Remove(key) &&
      !impl.reserve_entity_ids_response_callbacks.Remove(key) &&
      !impl.create_entity_response_callbacks.Remove(key) &&
      !impl.delete_entity_response_callbacks.Remove(key) &&
      !impl.entity_query_response_callbacks.Remove(key) &&
      !impl.add_component_callbacks.Remove(key) && !impl.remove_component_callbacks.Remove(key) &&
      !impl.authority_change_callbacks.Remove(key) &&
      !impl.component_update_callbacks.Remove(key) && !impl.command_request_callbacks.Remove(key) &&
      !impl.command_response_callbacks.Remove(key)) {
    std::terminate();
  }
}

inline void Dispatcher::Process(const OpList& op_list) const {
  for (std::uint32_t op_index = 0; op_index < op_list.op_list->OpCount; ++op_index) {
    auto& op = op_list.op_list->Op[op_index];
    switch (op.OpType) {
    case detail::internal::WORKER_PROTOCOL_OP_TYPE_DISCONNECT:
      detail::HandleDisconnect(impl, op.Disconnect);
      break;
    case detail::internal::WORKER_PROTOCOL_OP_TYPE_FLAG_UPDATE:
      detail::HandleFlagUpdate(impl, op.FlagUpdate);
      break;
    case detail::internal::WORKER_PROTOCOL_OP_TYPE_LOG_MESSAGE:
      detail::HandleLogMessage(impl, op.LogMessage);
      break;
    case detail::internal::WORKER_PROTOCOL_OP_TYPE_METRICS:
      detail::HandleMetrics(impl, op.Metrics);
      break;
    case detail::internal::WORKER_PROTOCOL_OP_TYPE_CRITICAL_SECTION:
      detail::HandleCriticalSection(impl, op.CriticalSection);
      break;
    case detail::internal::WORKER_PROTOCOL_OP_TYPE_ADD_ENTITY:
      detail::HandleAddEntity(impl, op.AddEntity);
      break;
    case detail::internal::WORKER_PROTOCOL_OP_TYPE_REMOVE_ENTITY:
      detail::HandleRemoveEntity(impl, op.RemoveEntity);
      break;
    case detail::internal::WORKER_PROTOCOL_OP_TYPE_RESERVE_ENTITY_ID_RESPONSE:
      detail::HandleReserveEntityIdResponse(impl, op.ReserveEntityIdResponse);
      break;
    case detail::internal::WORKER_PROTOCOL_OP_TYPE_RESERVE_ENTITY_IDS_RESPONSE:
      detail::HandleReserveEntityIdsResponse(impl, op.ReserveEntityIdsResponse);
      break;
    case detail::internal::WORKER_PROTOCOL_OP_TYPE_CREATE_ENTITY_RESPONSE:
      detail::HandleCreateEntityResponse(impl, op.CreateEntityResponse);
      break;
    case detail::internal::WORKER_PROTOCOL_OP_TYPE_DELETE_ENTITY_RESPONSE:
      detail::HandleDeleteEntityResponse(impl, op.DeleteEntityResponse);
      break;
    case detail::internal::WORKER_PROTOCOL_OP_TYPE_ENTITY_QUERY_RESPONSE:
      detail::HandleEntityQueryResponse(impl, op.EntityQueryResponse);
      break;
    case detail::internal::WORKER_PROTOCOL_OP_TYPE_ADD_COMPONENT:
      detail::HandleAddComponent(impl, op.AddComponent);
      break;
    case detail::internal::WORKER_PROTOCOL_OP_TYPE_REMOVE_COMPONENT:
      detail::HandleRemoveComponent(impl, op.RemoveComponent);
      break;
    case detail::internal::WORKER_PROTOCOL_OP_TYPE_AUTHORITY_CHANGE:
      detail::HandleAuthorityChange(impl, op.AuthorityChange);
      break;
    case detail::internal::WORKER_PROTOCOL_OP_TYPE_COMPONENT_UPDATE:
      detail::HandleComponentUpdate(impl, op.ComponentUpdate);
      break;
    case detail::internal::WORKER_PROTOCOL_OP_TYPE_COMMAND_REQUEST:
      detail::HandleCommandRequest(impl, op.CommandRequest);
      break;
    case detail::internal::WORKER_PROTOCOL_OP_TYPE_COMMAND_RESPONSE:
      detail::HandleCommandResponse(impl, op.CommandResponse);
      break;
    }
  }
}

}  // ::worker

#endif  // WORKER_SDK_CPP_INCLUDE_IMPROBABLE_DETAIL_DISPATCHER_I_H
