/**
 * @file jobs_dot.h
 * @brief DOT graph generation utilities for job dependency graphs.
 *
 * This file provides functionality to export job dependency graphs to DOT format,
 * which can be visualized using Graphviz. The DOT format represents directed graphs
 * as text, making it useful for debugging and documentation of job dependencies.
 */
#pragma once

#include "core_dynstring.h"
#include "jobs_graph.h"

typedef struct sFile File;

/**
 * @brief Write a job graph to a dynamic string in DOT format.
 *
 * Serializes the job dependency graph into DOT format and appends it to the
 * provided dynamic string. The resulting DOT representation can be used with
 * Graphviz tools to visualize the job dependency structure.
 *
 * @param str Dynamic string to append the DOT representation to
 * @param graph Job graph to serialize
 */
void jobs_dot_write_graph(DynString* str, JobGraph* graph);

/**
 * @brief Write a job graph to a file in DOT format.
 *
 * Serializes the job dependency graph into DOT format and writes it directly
 * to the specified file. This is a convenience function for writing DOT
 * representations to disk for visualization or analysis.
 *
 * @param file File handle to write the DOT representation to
 * @param graph Job graph to serialize
 */
void jobs_dot_dump_graph(File* file, JobGraph* graph);