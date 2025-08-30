#include "core_alloc.h"

#include "jobs_graph.h"

#include "anvil_spec.h"

spec(graph) {
    it("stores a graph name") {
        JobGraph* job = jobs_graph_create(g_alloc_heap, string_lit("TestJob"), 0);
        anvil_eq_string(jobs_graph_name(job), string_lit("TestJob"));
        jobs_graph_destroy(job);
    }

    it("stores task names") {
        JobGraph* job = jobs_graph_create(g_alloc_heap, string_lit("TestJob"), 2);

        const JobTaskId taskA = jobs_graph_add_task(job, string_lit("TestTaskA"), null, mem_empty);
        const JobTaskId taskB = jobs_graph_add_task(job, string_lit("TestTaskB"), null, mem_empty);

        anvil_eq_u64(jobs_graph_task_count(job), 2);
        anvil_eq_string(jobs_graph_task_name(job, taskA), string_lit("TestTaskA"));
        anvil_eq_string(jobs_graph_task_name(job, taskB), string_lit("TestTaskB"));

        jobs_graph_destroy(job);
    }

    it("supports graphs with many-to-one dependencies") {
        JobGraph* job = jobs_graph_create(g_alloc_heap, string_lit("TestJob"), 2);

        const JobTaskId a = jobs_graph_add_task(job, string_lit("A"), null, mem_empty);
        const JobTaskId b = jobs_graph_add_task(job, string_lit("B"), null, mem_empty);
        const JobTaskId c = jobs_graph_add_task(job, string_lit("C"), null, mem_empty);
        const JobTaskId d = jobs_graph_add_task(job, string_lit("D"), null, mem_empty);

        anvil_eq_u64(jobs_graph_task_count(job), 4);

        jobs_graph_task_depend(job, a, d);
        jobs_graph_task_depend(job, b, d);
        jobs_graph_task_depend(job, c, d);

        anvil_eq_u64(jobs_graph_task_span(job), 2);
        anvil(jobs_graph_validate(job));
        anvil_eq_u64(jobs_graph_task_root_count(job), 3);
        anvil_eq_u64(jobs_graph_task_leaf_count(job), 1);

        anvil(jobs_graph_task_has_parent(job, d));
        anvil(!jobs_graph_task_has_parent(job, a));
        anvil(!jobs_graph_task_has_parent(job, b));
        anvil(!jobs_graph_task_has_parent(job, c));

        anvil(jobs_graph_task_has_child(job, a));
        anvil(jobs_graph_task_has_child(job, b));
        anvil(jobs_graph_task_has_child(job, c));
        anvil(!jobs_graph_task_has_child(job, d));

        anvil_eq_u64(jobs_graph_task_child_begin(job, a).task, d);
        anvil_eq_u64(jobs_graph_task_child_begin(job, b).task, d);
        anvil_eq_u64(jobs_graph_task_child_begin(job, c).task, d);
        anvil(sentinel_check(jobs_graph_task_child_begin(job, d).task));

        jobs_graph_destroy(job);
    }

    it("supports graphs with one-to-many dependencies") {
        JobGraph* job = jobs_graph_create(g_alloc_heap, string_lit("TestJob"), 2);

        const JobTaskId a = jobs_graph_add_task(job, string_lit("A"), null, mem_empty);
        const JobTaskId b = jobs_graph_add_task(job, string_lit("B"), null, mem_empty);
        const JobTaskId c = jobs_graph_add_task(job, string_lit("C"), null, mem_empty);
        const JobTaskId d = jobs_graph_add_task(job, string_lit("D"), null, mem_empty);

        anvil_eq_u64(jobs_graph_task_count(job), 4);

        jobs_graph_task_depend(job, a, b);
        jobs_graph_task_depend(job, a, c);
        jobs_graph_task_depend(job, a, d);

        anvil(jobs_graph_validate(job));
        anvil_eq_u64(jobs_graph_task_span(job), 2);
        anvil_eq_u64(jobs_graph_task_root_count(job), 1);
        anvil_eq_u64(jobs_graph_task_leaf_count(job), 3);

        anvil(!jobs_graph_task_has_parent(job, a));
        anvil(jobs_graph_task_has_parent(job, b));
        anvil(jobs_graph_task_has_parent(job, c));
        anvil(jobs_graph_task_has_parent(job, d));

        anvil(jobs_graph_task_has_child(job, a));
        anvil(!jobs_graph_task_has_child(job, b));
        anvil(!jobs_graph_task_has_child(job, c));
        anvil(!jobs_graph_task_has_child(job, d));

        JobTaskChildItr itr = jobs_graph_task_child_begin(job, a);
        anvil_eq_u64(itr.task, b);
        itr = jobs_graph_task_child_next(job, itr);
        anvil_eq_u64(itr.task, c);
        itr = jobs_graph_task_child_next(job, itr);
        anvil_eq_u64(itr.task, d);
        itr = jobs_graph_task_child_next(job, itr);
        anvil(sentinel_check(itr.task));

        jobs_graph_destroy(job);
    }

    it("can detect cycles") {
        JobGraph* job = jobs_graph_create(g_alloc_heap, string_lit("TestJob"), 2);

        const JobTaskId a = jobs_graph_add_task(job, string_lit("A"), null, mem_empty);
        const JobTaskId b = jobs_graph_add_task(job, string_lit("B"), null, mem_empty);

        jobs_graph_task_depend(job, a, b);
        jobs_graph_task_depend(job, b, a);

        anvil(!jobs_graph_validate(job));

        jobs_graph_destroy(job);
    }

    it("can detect indirect cycles") {
        JobGraph* job = jobs_graph_create(g_alloc_heap, string_lit("TestJob"), 2);

        const JobTaskId a = jobs_graph_add_task(job, string_lit("A"), null, mem_empty);
        const JobTaskId b = jobs_graph_add_task(job, string_lit("B"), null, mem_empty);
        const JobTaskId c = jobs_graph_add_task(job, string_lit("C"), null, mem_empty);
        const JobTaskId d = jobs_graph_add_task(job, string_lit("D"), null, mem_empty);
        const JobTaskId e = jobs_graph_add_task(job, string_lit("E"), null, mem_empty);
        const JobTaskId f = jobs_graph_add_task(job, string_lit("F"), null, mem_empty);
        const JobTaskId g = jobs_graph_add_task(job, string_lit("G"), null, mem_empty);

        jobs_graph_task_depend(job, a, b);
        jobs_graph_task_depend(job, a, c);
        jobs_graph_task_depend(job, b, d);
        jobs_graph_task_depend(job, c, d);
        jobs_graph_task_depend(job, d, e);
        jobs_graph_task_depend(job, f, e);
        jobs_graph_task_depend(job, g, d);
        jobs_graph_task_depend(job, e, c);

        anvil(!jobs_graph_validate(job));

        jobs_graph_destroy(job);
    }

    it("can compute the span of a serial chain") {
        JobGraph* job = jobs_graph_create(g_alloc_heap, string_lit("TestJob"), 2);

        const JobTaskId a = jobs_graph_add_task(job, string_lit("A"), null, mem_empty);
        const JobTaskId b = jobs_graph_add_task(job, string_lit("B"), null, mem_empty);
        const JobTaskId c = jobs_graph_add_task(job, string_lit("C"), null, mem_empty);
        const JobTaskId d = jobs_graph_add_task(job, string_lit("D"), null, mem_empty);
        const JobTaskId e = jobs_graph_add_task(job, string_lit("E"), null, mem_empty);
        const JobTaskId f = jobs_graph_add_task(job, string_lit("F"), null, mem_empty);
        const JobTaskId g = jobs_graph_add_task(job, string_lit("G"), null, mem_empty);

        jobs_graph_task_depend(job, a, b);
        jobs_graph_task_depend(job, b, c);
        jobs_graph_task_depend(job, c, d);
        jobs_graph_task_depend(job, d, e);
        jobs_graph_task_depend(job, e, f);
        jobs_graph_task_depend(job, f, g);

        anvil(jobs_graph_validate(job));
        anvil_eq_u64(jobs_graph_task_span(job), 7);
        anvil_eq_u64(jobs_graph_task_root_count(job), 1);
        anvil_eq_u64(jobs_graph_task_leaf_count(job), 1);

        jobs_graph_destroy(job);
    }

    it("can compute the span of a parallel chain") {
        JobGraph* job = jobs_graph_create(g_alloc_heap, string_lit("TestJob"), 2);

        jobs_graph_add_task(job, string_lit("A"), null, mem_empty);
        jobs_graph_add_task(job, string_lit("B"), null, mem_empty);
        jobs_graph_add_task(job, string_lit("C"), null, mem_empty);
        jobs_graph_add_task(job, string_lit("D"), null, mem_empty);
        jobs_graph_add_task(job, string_lit("E"), null, mem_empty);
        jobs_graph_add_task(job, string_lit("F"), null, mem_empty);
        jobs_graph_add_task(job, string_lit("G"), null, mem_empty);

        anvil(jobs_graph_validate(job));
        anvil_eq_u64(jobs_graph_task_span(job), 1);
        anvil_eq_u64(jobs_graph_task_root_count(job), 7);
        anvil_eq_u64(jobs_graph_task_leaf_count(job), 7);

        jobs_graph_destroy(job);
    }

    it("can compute the span of a complex chain") {
        JobGraph* job = jobs_graph_create(g_alloc_heap, string_lit("TestJob"), 2);

        const JobTaskId a = jobs_graph_add_task(job, string_lit("A"), null, mem_empty);
        const JobTaskId b = jobs_graph_add_task(job, string_lit("B"), null, mem_empty);
        const JobTaskId c = jobs_graph_add_task(job, string_lit("C"), null, mem_empty);
        const JobTaskId d = jobs_graph_add_task(job, string_lit("D"), null, mem_empty);
        const JobTaskId e = jobs_graph_add_task(job, string_lit("E"), null, mem_empty);
        const JobTaskId f = jobs_graph_add_task(job, string_lit("F"), null, mem_empty);
        const JobTaskId g = jobs_graph_add_task(job, string_lit("G"), null, mem_empty);
        const JobTaskId h = jobs_graph_add_task(job, string_lit("H"), null, mem_empty);
        const JobTaskId i = jobs_graph_add_task(job, string_lit("I"), null, mem_empty);
        const JobTaskId j = jobs_graph_add_task(job, string_lit("J"), null, mem_empty);
        const JobTaskId k = jobs_graph_add_task(job, string_lit("K"), null, mem_empty);
        const JobTaskId l = jobs_graph_add_task(job, string_lit("L"), null, mem_empty);
        const JobTaskId m = jobs_graph_add_task(job, string_lit("M"), null, mem_empty);
        const JobTaskId n = jobs_graph_add_task(job, string_lit("N"), null, mem_empty);
        const JobTaskId o = jobs_graph_add_task(job, string_lit("O"), null, mem_empty);
        const JobTaskId p = jobs_graph_add_task(job, string_lit("P"), null, mem_empty);
        const JobTaskId q = jobs_graph_add_task(job, string_lit("Q"), null, mem_empty);
        const JobTaskId r = jobs_graph_add_task(job, string_lit("R"), null, mem_empty);

        jobs_graph_task_depend(job, a, b);
        jobs_graph_task_depend(job, b, c);
        jobs_graph_task_depend(job, b, n);
        jobs_graph_task_depend(job, c, d);
        jobs_graph_task_depend(job, c, e);
        jobs_graph_task_depend(job, e, f);
        jobs_graph_task_depend(job, e, g);
        jobs_graph_task_depend(job, f, h);
        jobs_graph_task_depend(job, g, i);
        jobs_graph_task_depend(job, d, j);
        jobs_graph_task_depend(job, j, k);
        jobs_graph_task_depend(job, h, k);
        jobs_graph_task_depend(job, i, k);
        jobs_graph_task_depend(job, k, l);
        jobs_graph_task_depend(job, l, m);
        jobs_graph_task_depend(job, n, q);
        jobs_graph_task_depend(job, n, r);
        jobs_graph_task_depend(job, q, o);
        jobs_graph_task_depend(job, r, p);
        jobs_graph_task_depend(job, o, m);
        jobs_graph_task_depend(job, p, m);

        anvil(jobs_graph_validate(job));
        anvil_eq_u64(jobs_graph_task_span(job), 9);
        anvil_eq_f64(jobs_graph_task_parallelism(job), 2.0f, 1e-6f);
        anvil_eq_u64(jobs_graph_task_root_count(job), 1);
        anvil_eq_u64(jobs_graph_task_leaf_count(job), 1);

        jobs_graph_destroy(job);
    }
}
