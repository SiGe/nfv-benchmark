#----------------------------------------
# Elements
#----------------------------------------
fastpass() {
    buffer=$1
    cat - <<EOF
    pipeline_element_add(pipe, el_${ELEMENT_PREFIX}fastpass_create(MOD_BUFFER_SIZE_${buffer}, stream));
EOF
}

bp_checksum() {
    buffer=$1
    cat - <<EOF
    pipeline_element_add(pipe, el_${ELEMENT_PREFIX}bp_checksum_create(MOD_BUFFER_SIZE_${buffer}));
EOF
}

bpp_checksum() {
    buffer=$1
    cat - <<EOF
    pipeline_element_add(pipe, el_${ELEMENT_PREFIX}bp_checksum_create(MOD_BUFFER_SIZE_${buffer}));
EOF
}


batching_checksum() {
    buffer=$1
    cat - <<EOF
    pipeline_element_add(pipe, el_${ELEMENT_PREFIX}batching_checksum_create(MOD_BUFFER_SIZE_${buffer}));
EOF
}

naive_checksum() {
    buffer=$1
    cat - <<EOF
    pipeline_element_add(pipe, el_only_naive_checksum_create());
EOF
}

checksum() {
    buffer=$1
    cat - <<EOF
    pipeline_element_add(pipe, el_${ELEMENT_PREFIX}checksum_create(MOD_BUFFER_SIZE_${buffer}));
EOF
}

drop() {
    cat - <<EOF
    pipeline_element_add(pipe, el_${ELEMENT_PREFIX}drop_mbuf_create(MOD_BUFFER_SIZE_DROP, stream));
EOF
}

timer() {
    buffer=$1
    cat - <<EOF
    pipeline_element_add(pipe, el_${ELEMENT_PREFIX}timer_create(MOD_BUFFER_SIZE_${buffer}));
EOF
}


identity() {
    buffer=$1
    cat - <<EOF
    pipeline_element_add(pipe, el_${ELEMENT_PREFIX}identity_create(MOD_BUFFER_SIZE_${buffer}));
EOF
}

bpp_measurement_small() {
    buffer=$1
    cat - <<EOF
    pipeline_element_add(pipe, el_${ELEMENT_PREFIX}bpp_measurement_create_with_size(MOD_BUFFER_SIZE_${buffer}, 
        1 << 18));
EOF
}

bp_measurement_small() {
    buffer=$1
    cat - <<EOF
    pipeline_element_add(pipe, el_${ELEMENT_PREFIX}bp_measurement_create_with_size(MOD_BUFFER_SIZE_${buffer}, 
        1 << 18));
EOF
}

batching_measurement_small() {
    buffer=$1
    cat - <<EOF
    pipeline_element_add(pipe, el_${ELEMENT_PREFIX}batching_measurement_create_with_size(MOD_BUFFER_SIZE_${buffer}, 
        1 << 18));
EOF
}

naive_measurement_small() {
    buffer=$1
    cat - <<EOF
    pipeline_element_add(pipe, el_only_naive_measurement_create_with_size(1 << 18));
EOF
}

measurement_small() {
    buffer=$1
    cat - <<EOF
    pipeline_element_add(pipe, el_${ELEMENT_PREFIX}measurement_create_with_size(MOD_BUFFER_SIZE_${buffer}, 
        1 << 18));
EOF
}

bpp_measurement_large() {
    buffer=$1
    cat - <<EOF
    pipeline_element_add(pipe, el_${ELEMENT_PREFIX}bpp_measurement_create_with_size(MOD_BUFFER_SIZE_${buffer}, 
        1 << 24));
EOF
}

bp_measurement_large() {
    buffer=$1
    cat - <<EOF
    pipeline_element_add(pipe, el_${ELEMENT_PREFIX}bp_measurement_create_with_size(MOD_BUFFER_SIZE_${buffer}, 
        1 << 24));
EOF
}

batching_measurement_large() {
    buffer=$1
    cat - <<EOF
    pipeline_element_add(pipe, el_${ELEMENT_PREFIX}batching_measurement_create_with_size(MOD_BUFFER_SIZE_${buffer}, 
        1 << 24));
EOF
}

naive_measurement_large() {
    buffer=$1
    cat - <<EOF
    pipeline_element_add(pipe, el_only_naive_measurement_create_with_size(1 << 24));
EOF
}

measurement_large() {
    buffer=$1
    cat - <<EOF
    pipeline_element_add(pipe, el_${ELEMENT_PREFIX}measurement_create_with_size(MOD_BUFFER_SIZE_${buffer}, 
        1 << 24));
EOF
}

bpp_routing_stanford() {
    buffer=$1
    cat - <<EOF
    pipeline_element_add(pipe, el_${ELEMENT_PREFIX}bpp_routing_create_with_file(MOD_BUFFER_SIZE_${buffer}, 
        "data/boza_rtr_route.lpm"));
EOF
}

bp_routing_stanford() {
    buffer=$1
    cat - <<EOF
    pipeline_element_add(pipe, el_${ELEMENT_PREFIX}bp_routing_create_with_file(MOD_BUFFER_SIZE_${buffer}, 
        "data/boza_rtr_route.lpm"));
EOF
}

batching_routing_stanford() {
    buffer=$1
    cat - <<EOF
    pipeline_element_add(pipe, el_${ELEMENT_PREFIX}batching_routing_create_with_file(MOD_BUFFER_SIZE_${buffer}, 
        "data/boza_rtr_route.lpm"));
EOF
}

naive_routing_stanford() {
    buffer=$1
    cat - <<EOF
    pipeline_element_add(pipe, el_only_naive_routing_create_with_file("data/boza_rtr_route.lpm"));
EOF
}

routing_stanford() {
    buffer=$1
    cat - <<EOF
    pipeline_element_add(pipe, el_${ELEMENT_PREFIX}routing_create_with_file(MOD_BUFFER_SIZE_${buffer}, 
        "data/boza_rtr_route.lpm"));
EOF
}

merged() {
    buffer=$1
    cat - <<EOF
    pipeline_element_add(pipe, el_${ELEMENT_PREFIX}merged_create(MOD_BUFFER_SIZE_${buffer}, 
        "data/boza_rtr_route.lpm", 1 << 24));
EOF
}

merged_opt() {
    buffer=$1
    cat - <<EOF
    pipeline_element_add(pipe, el_${ELEMENT_PREFIX}merged_opt_create(MOD_BUFFER_SIZE_${buffer}, 
        "data/boza_rtr_route.lpm", 1 << 18));
EOF
}

merged_fastpass() {
    buffer=$1
    cat - <<EOF
    pipeline_element_add(pipe, el_${ELEMENT_PREFIX}merged_fastpass_create(MOD_BUFFER_SIZE_${buffer}, 
        "data/boza_rtr_route.lpm", 1 << 24));
EOF
}

