# Module Interface

Each module implements a few functions (``_create, _process, _release,
_report``).  The most important one being \_process.  Module's ``_process``
accepts a batch of packets (read from the NIC) and processes them in whatever
way that is appropriate.  The interface is as follows:

``
*_process(struct element_t *ele, struct packet_t **pkts, packet_index_t size)
``

The batch size (number of packets read from the NIC) does impact the
performance.  I did a sweep for different sizes and IIRC a batch size of 90~120
is at the knee point of throughput/latency curve.  Although, the optimal batch
size does depend on the code.

The framework itself is modular and mimics the internal of Click modular
router.  I didn't use Click because it has a high overhead when dealing with
small packets.  It also has no notion of batch (in the default version) and it
is more complex to change the "batch" size or change how the packets are
processed.

# Elements

For each of the 3 modules (Checksum, Routing, Measurement), we have 5 default
implementations and 2 amalgamated implementations (i.e., the three modules are
combined):

- ```naive_*.c (e.g., src/elements/naive_measurement.c)```: this is the base implementation and the one that isn't optimized.  It processes one packet at a time (as a naive programmer would do when writing code).

- ```batching_*.c (e.g., src/elements/batching_measurement.c)```: This looks exactly like the naive implementation with one difference: it processes packets in batches.

- ```bp_*.c (e.g., src/elements/bp_measurement.c)```: Similar to batching but it also prefetches random access data (only valid for measurement module).

- ```bpp_*.c (e.g., src/elements/bpp_measurement.c)```:  Packet data batching and whatever ```bp_``` was doing.

- ```*.c (e.g., src/elements/measurement.c)```: This is where things get a little bit crazy.  I have to help the compiler unroll the loops by using fixed loop strides. To unroll the loops, I break down a batch of packets that I get into smaller batches of size 8 or 16.  Then loop over fixed batch sizes of 8 or 16 and process packets.  I could have used a struct similar to a duff's device, but I didn't have the chance to try it.  This drastically improves the performance at the cost of additional code size.  To be more concrete, for the measurement task, when we receive a batch of 43 packets, I prefetch the first 8 packets, then in a loop prefetch the second 8 packets, process the first 8 packets, prefetch the third 8 packets, process the second 8 packets, and so on (using e as execution and p as prefetching): ``1p, 2p, 1e, 3p, 2e, 4p, 3e, 5p, 4e, 6p*, 5e, 6e*``, where the 6th batch has 3 packets only.

- ``merged.c``: This is basically the interleaving of all the modules.
- ``merged_fastpass.c``: This is interleaving of all modules + fastpass/slowpath separation. 

