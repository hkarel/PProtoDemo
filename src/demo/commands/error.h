#pragma once

#include "pproto/commands_base.h"

namespace pproto {

typedef data::MessageError ErrorInfo;

namespace error {

/*** Примеры ошибок взяты с реальной задачи ***/

//--- 10 Ошибки по работе с базой данных ---
DECL_ERROR_CODE(connect_to_database,          10, "2361b42b-7324-4230-8d00-a1c9348d7a58", u8"Ошибка подключения к базе данных")
DECL_ERROR_CODE(begin_transaction,            10, "d521450f-73f9-49a0-afe8-831dd66af353", u8"Ошибка старта транзакции")
DECL_ERROR_CODE(commit_transaction,           10, "5bc8301e-c1c5-4fa6-aeb8-c1b7b86afefe", u8"Ошибка завершения транзакции")
DECL_ERROR_CODE(rollback_transaction,         10, "ee3f3100-4643-4adf-8c6b-ca823e460d60", u8"Ошибка отмены транзакции")

DECL_ERROR_CODE(select_sql_statement,         10, "f99e821c-7f40-4c6e-aea2-b64ab5ec064c", u8"Ошибка выполнения 'select' sql-запроса")
DECL_ERROR_CODE(insert_sql_statement,         10, "141463a6-6cce-41c5-8a05-2473152ffe57", u8"Ошибка выполнения 'insert' sql-запроса")
DECL_ERROR_CODE(update_sql_statement,         10, "76861790-a004-4170-82e7-3ad608448645", u8"Ошибка выполнения 'update' sql-запроса")
DECL_ERROR_CODE(insert_or_update_sql,         10, "a1ba61bd-8c6d-464a-83e0-1db0d3f25246", u8"Ошибка выполнения 'insert or update' sql-запроса")
DECL_ERROR_CODE(delete_sql_statement,         10, "4ea9c633-0dec-4922-bdf7-3c5fa65f1075", u8"Ошибка выполнения 'delete' sql-запроса")

//--- 20 Ошибки общего плана ---
DECL_ERROR_CODE(labeled_dir_not_found,        20, "d1d8705a-89a9-4dce-a332-3019f47a6391", u8"Директория labeled_tables не найдена")
DECL_ERROR_CODE(client_already_connected,     20, "5a5c4de0-08e3-49c7-8152-2026758c5048", u8"Клиент с id %1 уже подключен. Socket descriptor: %2")
DECL_ERROR_CODE(docker_already_connected,     20, "e2dc1d05-d0bd-485b-9c2d-33e8773a92fa", u8"Docker с id %1 уже подключен. Socket descriptor: %2")

//--- 30 Ошибки по работе с задачами ---
DECL_ERROR_CODE(delete_run_task,              30, "2ac2a3b7-0568-4268-bab0-cdcd65d6d325", u8"Невозможно удалить работающую задачу. Остановите задачу и повторите действие")
DECL_ERROR_CODE(rerun_run_task,               30, "1dae6d75-9e9b-4331-9412-902173d6fa27", u8"Невозможно перезапустить работающую задачу")
DECL_ERROR_CODE(cancel_not_run_task,          30, "c936a2f9-510c-4229-aae2-ecd5440c5300", u8"Невозможно остановить не работающую задачу")
DECL_ERROR_CODE(return_not_complete_task,     30, "5877e71c-a26b-481c-a12b-19c9d86e074d", u8"Невозможно получить данные для незавершенной задачи")
DECL_ERROR_CODE(lost_docker_handler_task,     30, "dfb2ea35-5cda-4676-a6de-44d072520829", u8"Потерян docker-обработчик для задачи")

//--- 40 Ошибки по работе с proxy ---
DECL_ERROR_CODE(connect_to_service,           40, "e91e354d-8190-4130-ae66-b1e46c68c158", u8"Proxy: Нет подключения к tablematch сервису")
DECL_ERROR_CODE(lost_connect_to_service,      40, "1a5a0504-23d9-4c7b-9f9d-fae8792ddf1d", u8"Proxy: Потеряно подключения к tablematch сервису")

//--- 50 Ошибки по работе с json ---
DECL_ERROR_CODE(json_parse_error,             50, "fd665c40-c2e1-4697-b673-b15e6db9c846", "Ошибка разбора json-выражения")

//--- 60 Ошибки по работе с примерами ---
DECL_ERROR_CODE(tdemo02_04_bad,               60, "217f6c84-5981-49f4-a30d-f6a2f3a3ab84", "Bad executed demo-command TDemo02_04")



} // namespace error
} // namespace pproto
