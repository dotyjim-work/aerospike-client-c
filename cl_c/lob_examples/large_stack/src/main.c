/*  Citrusleaf General Performance Test Program
 *
 *  Tailored for the  Large Object Stack Test
 *  main.c - Basic Large Stack Performance Test
 *
 *  Copyright 2013 by Citrusleaf, Aerospike Inc.  All rights reserved.
 *  THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE.  THE COPYRIGHT NOTICE
 *  ABOVE DOES NOT EVIDENCE ANY ACTUAL OR INTENDED PUBLICATION.
 */

#include "test.h"
#include "counter.h"

// Global Configuration object: holds client config data.
test_config *g_config = NULL;

/** ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
 *  Show Usage
 */
void usage(int argc, char *argv[]) {
    INFO("Usage %s:", argv[0]);
    INFO("   -h host [default 127.0.0.1] ");
    INFO("   -p port [default 3000]");
    INFO("   -n namespace [default test]");
    INFO("   -s set [default *all*]");
}

/** ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
 *  Set up the configuration for the Test Run Routines
 */
int init_configuration (int argc, char *argv[]) {
    static char * meth = "init_configuration()";

    // NOTE: Can't do INFO() until AFTER g_config is allocated.
    printf("[ENTER]:[%s]: Num Args (%d)\n", meth, argc );

    g_config = (test_config *)malloc(sizeof(test_config));
    memset(g_config, 0, sizeof(test_config));

    set_config_defaults( g_config );

    INFO("[DEBUG]:[%s]: Num Args (%d) g_config(%p)\n",
            meth, argc, g_config);

    INFO("[DEBUG]:[%s]: About to Process Args (%d)\n", meth, argc );
    int optcase;
    while ((optcase = getopt(argc, argv, "ckmh:p:n:s:P:f:v:x:r:t:i:j:")) != -1){
        INFO("[ENTER]:[%s]: Processings Arg(%d)\n", meth, optcase );
        switch (optcase) {
        case 'h': g_config->host    = strdup(optarg); break;
        case 'p': g_config->port    = atoi(optarg);   break;
        case 'n': g_config->ns      = strdup(optarg); break;
        case 's': g_config->set     = strdup(optarg); break;
        case 'v': g_config->verbose = true;           break;
        case 't': g_config->n_threads = atoi(optarg);   break;
        case 'i': g_config->n_iterations = atoi(optarg);   break;
        default:  usage(argc, argv);                  return(-1);
        }
    }
    return 0;
}

// Invoke the program that runs in the thread.  Pass in the address of
// a simple integer (the logical thread number).
//
// Not sure why this has to be "static", but perhaps that's how the
// functions are invoked for a thread.
static void *run_test(void *o) {
    char user_key[30];
    unsigned int thread_num;
    if( o != NULL ) {
        thread_num = *(unsigned int *) o;
    }  
    
    srand( thread_num ); // make every thread start differently
    unsigned int random_num = rand() % 100;

    sprintf(user_key, "User_%d", random_num );
    printf(">>>>>>>>> RUN TEST 1 <<<<<<<<<<<< (user key[%s])\n", user_key);
    printf(">>>>>>>>> RUN TEST 1 <<<<<<<<<<<<\n");
    run_test1(user_key);

//    sprintf(user_key, "User_%d", rand()%100);
//    run_test2(user_key);

    printf(">>>>>>>>> RUN TEST 3 <<<<<<<<<<<<\n");
    printf(">>>>>>>>> RUN TEST 3 <<<<<<<<<<<<\n");
    run_test3( random_num );

    return NULL;
}

// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
/**
 * Print out our counters (maybe this belongs in Config?)
 */
void print_counters() {
    fprintf(stderr, "TEST(FN): Total Keys(%"PRIu64") \n",
        cf_atomic_int_get(g_config->key_counter->val) );

    fprintf(stderr, ">> Read Ops(%"PRIu64") Read Vals(%"PRIu64") \n",
        cf_atomic_int_get(g_config->read_ops_counter->val),
        cf_atomic_int_get(g_config->read_vals_counter->val) );

    fprintf(stderr, ">> Write Ops(%"PRIu64") Write Vals(%"PRIu64") \n",
        cf_atomic_int_get(g_config->write_ops_counter->val),
        cf_atomic_int_get(g_config->write_vals_counter->val) );

    fprintf(stderr, ">> Delete Ops(%"PRIu64") Delete Vals(%"PRIu64") \n",
        cf_atomic_int_get(g_config->delete_ops_counter->val),
        cf_atomic_int_get(g_config->delete_vals_counter->val) );

} // end print_stats()


/** ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
*  This file exercises the Aerospike Interface.
*  For a particular TestSuite, it will exercise various test scripts
*  under a variety of conditions, and measure it while doing so.
*
*  Launch "T" Threads
*  Each Thread will perform a set of operations for each User (record key).
*  Each Set of operations will follow one of the many scripts defined here.
*/
int main(int argc, char **argv) {
static char * meth         = "main()";
    int           rc           = 0;
    char * test_name = "LSO Test Run";

    // Run this FIRST THING
    init_configuration( argc, argv );

    INFO("[ENTER]:[%s]: Start in main()\n", meth );

    INFO("[DEBUG]:[%s]: calling setup_test()\n", meth );
    if (setup_test( argc, argv )) {
        return 0;
    }

    printf("<< Test Run >> Start (%s)\n", test_name );

    // Start our stopwatch for this test run
    uint64_t start_time = cf_getms();

    // Set up for operation counting;
    g_config->read_ops_counter     = atomic_int_create((uint64_t) 0);
    g_config->read_vals_counter    = atomic_int_create((uint64_t) 0);
    g_config->write_ops_counter    = atomic_int_create((uint64_t) 0);
    g_config->write_vals_counter   = atomic_int_create((uint64_t) 0);
    g_config->delete_ops_counter   = atomic_int_create((uint64_t) 0);
    g_config->delete_vals_counter  = atomic_int_create((uint64_t) 0);
    g_config->key_counter          = atomic_int_create((uint64_t) 0);
    g_config->success_counter      = atomic_int_create((uint64_t) 0);
    g_config->fail_counter         = atomic_int_create((uint64_t) 0);
    
    void *counter_control = start_test_counter_thread(
            g_config->read_ops_counter, g_config->read_vals_counter,
            g_config->write_ops_counter, g_config->write_vals_counter,
            g_config->delete_ops_counter, g_config->delete_vals_counter,
            g_config->key_counter);
    
    pthread_t slaps[g_config->n_threads];
    // Invoke the "run_test()" method for g_config->n_threads many threads
    for (int j = 0; j < g_config->n_threads; j++) {
        if (pthread_create(&slaps[j], 0, run_test, &j )) {
            INFO("[WARNING]: Thread Create Failed\n");
        }
    }
    for (int j = 0; j < g_config->n_threads; j++) {
        pthread_join(slaps[j], (void *)&rc);
    }

    // End Performance Testing
    stop_test_counter_thread(counter_control);

    uint64_t stop_time = cf_getms();


    printf("<< Test Run >> End (%s) \n", test_name );

    fprintf(stderr,"[LSO Test Run] Stop: Total Ops(%d) Time Elapsed (%lu)ms\n",
            (g_config->n_threads * g_config->n_iterations),
            (stop_time - start_time) );

    print_counters();

    printf("CITRUSLEAF STATS Follows ... \n");
    citrusleaf_print_stats();

    shutdown_test();
    exit(0);
} // end main()

