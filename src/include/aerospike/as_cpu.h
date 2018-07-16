/*
 * Copyright 2008-2018 Aerospike, Inc.
 *
 * Portions may be licensed to Aerospike, Inc. under one or more contributor
 * license agreements.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */
#pragma once 

#include <pthread.h>

#if defined(__APPLE__)
#include <mach/thread_policy.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 * FUNCTIONS
 *****************************************************************************/

/**
 * Assign a thread to a specific cpu core.
 */
static inline bool
as_cpu_assign_thread(pthread_t thread, int cpu_id)
{
	int rv;
#if defined(__APPLE__)
	thread_affinity_policy_data_t policy = {cpu_id};
	thread_port_t mach_thread = pthread_mach_thread_np(thread);
	rv = thread_policy_set(mach_thread, THREAD_AFFINITY_POLICY, (thread_policy_t)&policy, 1);
#else
	cpu_set_t cpuset;
	CPU_ZERO(&cpuset);
	CPU_SET(cpu_id, &cpuset);
	rv = pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
#endif
	return (rv == 0) ? true : false;
}

#ifdef __cplusplus
} // end extern "C"
#endif