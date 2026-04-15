// Micro-bench: calc_witness (re-parse every call, WTNS out) vs prepare_graph
// + calc_witness_raw_prepared (parse once, reuse, raw field-element out — the
// server hot path).
//
// Usage: cargo run --release --example bench_prepared -- <graph.bin> <inputs.json> [runs]

use std::env;
use std::fs;
use std::time::Instant;

use circom_witnesscalc::{calc_witness, calc_witness_raw_prepared, prepare_graph};

fn main() {
    let args: Vec<String> = env::args().collect();
    if args.len() < 3 {
        eprintln!("Usage: {} <graph.bin> <inputs.json> [runs]", args[0]);
        std::process::exit(1);
    }
    let runs: usize = args.get(3).and_then(|s| s.parse().ok()).unwrap_or(5);

    let graph = fs::read(&args[1]).expect("read graph");
    let inputs = fs::read_to_string(&args[2]).expect("read inputs");

    println!("=== calc_witness (re-parses graph each call) ===");
    for i in 0..runs {
        let t0 = Instant::now();
        let _w = calc_witness(&inputs, &graph).expect("calc_witness");
        let ms = t0.elapsed().as_millis();
        println!("  run {}: {} ms", i + 1, ms);
    }

    println!("\n=== prepare_graph + calc_witness_raw_prepared ===");
    let tp = Instant::now();
    let graph = prepare_graph(&graph).expect("prepare_graph");
    println!("  prepare: {} ms (one-time)", tp.elapsed().as_millis());
    for i in 0..runs {
        let t0 = Instant::now();
        let _w = calc_witness_raw_prepared(&graph, &inputs).expect("calc_witness_raw_prepared");
        let ms = t0.elapsed().as_millis();
        println!("  run {}: {} ms", i + 1, ms);
    }
}
