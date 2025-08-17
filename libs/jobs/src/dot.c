/**
 * @file dot.c
 * @brief DOT graph generation implementation for job dependency graphs
 *
 * This file implements functionality to export job dependency graphs to DOT format,
 * which can be visualized using GraphViz tools. The implementation generates directed
 * graphs that show task dependencies with proper start/end nodes and task nodes.
 */

#include "core_alloc.h"
#include "core_dynstring.h"
#include "core_file.h"
#include "core_format.h"
#include "jobs_dot.h"

/**
 * @brief Shape definition for start nodes in DOT output
 */
#define dot_start_shape "octagon"

/**
 * @brief Shape definition for end nodes in DOT output
 */
#define dot_end_shape "octagon"

/**
 * @brief Shape definition for task nodes in DOT output
 */
#define dot_task_shape "box"

/**
 * @brief Writes a single task node definition to the DOT output
 *
 * @param str Dynamic string to write the DOT node definition to
 * @param graph Job graph containing the task
 * @param taskId ID of the task to write
 */
static void dot_write_task_node(DynString* str, JobGraph* graph, const JobTaskId taskId) {
    fmt_write(
        str,
        "  task_{} [label=\"{}\", shape=" dot_task_shape "];\n",
        fmt_int(taskId),
        fmt_text(jobs_graph_task_name(graph, taskId)));
}

/**
 * @brief Writes edges from a task to all its child tasks in DOT format
 *
 * If the task has no children, an edge to the "end" node is written instead.
 *
 * @param str Dynamic string to write the DOT edges to
 * @param graph Job graph containing the task relationships
 * @param taskId ID of the parent task
 */
static void dot_write_task_child_edges(DynString* str, JobGraph* graph, const JobTaskId taskId) {
    fmt_write(str, "  task_{} -> {", fmt_int(taskId));

    bool elemWritten = false;
    jobs_graph_for_task_child(graph, taskId, child, {
        fmt_write(
            str, "{}task_{}", elemWritten ? fmt_text_lit(", ") : fmt_nop(), fmt_int(child.task));
        elemWritten = true;
    });

    if (!elemWritten) {
        fmt_write(str, "end");
    }

    fmt_write(str, "};\n", fmt_int(taskId));
}

/**
 * @brief Writes edges from the start node to all root tasks (tasks with no parents)
 *
 * @param str Dynamic string to write the DOT edges to
 * @param graph Job graph to analyze for root tasks
 */
static void dot_write_start_task_edges(DynString* str, JobGraph* graph) {
    fmt_write(str, "  start -> {");

    bool elemWritten = false;
    jobs_graph_for_task(graph, taskId, {
        if (jobs_graph_task_has_parent(graph, taskId)) {
            continue;
        }

        fmt_write(str, "{}task_{}", elemWritten ? fmt_text_lit(", ") : fmt_nop(), fmt_int(taskId));
        elemWritten = true;
    });

    fmt_write(str, "}\n");
}

/**
 * @brief Writes a complete job graph in DOT format to a dynamic string
 *
 * Generates a complete directed graph showing all tasks and their dependencies,
 * with special start and end nodes. The output can be processed by GraphViz.
 *
 * @param str Dynamic string to write the DOT graph to
 * @param graph Job graph to convert to DOT format
 */
void jobs_dot_write_graph(DynString* str, JobGraph* graph) {
    fmt_write(str, "digraph {} {\n"
                   "  start [label=\"JobStart\", shape=" dot_start_shape "];\n"
                   "  end [label=\"JobEnd\", shape=" dot_end_shape "];\n\n",
              fmt_text(jobs_graph_name(graph)));

    jobs_graph_for_task(graph, taskId, { dot_write_task_node(str, graph, taskId); });
    fmt_write(str, "\n");

    dot_write_start_task_edges(str, graph);
    fmt_write(str, "\n");

    jobs_graph_for_task(graph, taskId, { dot_write_task_child_edges(str, graph, taskId); });
    fmt_write(str, "}\n");
}

/**
 * @brief Writes a job graph in DOT format directly to a file
 *
 * Convenience function that creates a temporary buffer, generates the DOT
 * representation, and writes it to the specified file.
 *
 * @param file File handle to write the DOT output to
 * @param graph Job graph to convert and write
 */
void jobs_dot_dump_graph(File* file, JobGraph* graph) {
    DynString buffer = dynstring_create(g_alloc_heap, usize_kibibyte);
    jobs_dot_write_graph(&buffer, graph);
    file_write_sync(file, dynstring_view(&buffer));
    dynstring_destroy(&buffer);
}