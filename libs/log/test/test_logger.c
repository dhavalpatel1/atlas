#include "core_alloc.h"

#include "anvil_spec.h"

#include "core_dynarray.h"
#include "core_sourceloc.h"
#include "core_time.h"

#include "log_logger.h"
#include "log_sink.h"

typedef struct {
    LogLevel level;
    String message;
    TimeReal timestamp;
} SinkTestMsg;

typedef struct {
    LogSink api;
    DynArray messages;
} SinkTest;

static void log_sink_test_write(LogSink* sink, LogLevel level, SourceLoc srcLoc, TimeReal timestamp, String message, const LogParam* params) {
    SinkTest* testSink = (SinkTest*)sink;

    (void)sink;
    (void)srcLoc;
    (void)timestamp;
    (void)params;

    *dynarray_push_t(&testSink->messages, SinkTestMsg) = (SinkTestMsg) {
        .level = level,
        .message = string_dup(g_alloc_heap, message)
    };
}

spec(logger) {
    TimeReal startTime = 0;
    Logger* logger = null;
    SinkTest sink = {0};

    setup() {
        startTime = time_real_clock();
        logger = log_create(g_alloc_heap);
        sink = (SinkTest) {
            .api = (LogSink){
                .write = log_sink_test_write
            },
            .messages = dynarray_create_t(g_alloc_heap, SinkTestMsg, 8)
        };

        log_add_sink(logger, (LogSink*)&sink);
    }

    it("sends recieved message to attached sinks") {
        log(logger, LogLevel_Error, "Hello World");

        SinkTestMsg* msg = dynarray_at_t(&sink.messages, 0, SinkTestMsg);
        anvil_eq_u64(msg->level, LogLevel_Error);
        anvil_eq_string(msg->message, string_lit("Hello World"));
    }

    it("supports formatting the meeessage with parameters") {
        log(logger, LogLevel_Error, "Initializing {} stage {}", log_param("name", fmt_text_lit("System9000")), log_param("stage", fmt_int(2)));

        SinkTestMsg* msg = dynarray_at_t(&sink.messages, 0, SinkTestMsg);
        anvil_eq_string(msg->message, string_lit("Initializing System9000 stage 2"));
    }

    it("timestamps all messages") {
        log(logger, LogLevel_Info, "Hello World");

        SinkTestMsg* msg = dynarray_at_t(&sink.messages, 0, SinkTestMsg);
        anvil(time_real_duration(startTime, msg->timestamp) < time_second);
    }

    teardown() {
        log_destroy(logger);

        dynarray_for_t(&sink.messages, SinkTestMsg, msg, {
            string_free(g_alloc_heap, msg->message);
        });
        dynarray_destroy(&sink.messages);
    }
}
