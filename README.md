# Tea DSP

A platform for audio applications with a modular engine written in Rust.

### Why?

* I like rust
* Keeping to core algorithms / engine in modular makes it easier to extend one engine implementation to multiple use cases (standalone, vst, aax, etc.)
* **Solution**: Make the engine logic out-of-process using flatbuffers to keep messaging latency down.
* Alternatives were:
  * Write binding for C++, Swift, and any other language that I wanted to bind an sdk to the rust. This is time consuming and also prone to bugs (I've tried this a few times already).
  * Write bindings for any platform/framework I want to use. This is also extremely time consuming, with the added downside that many of the available tooling that's open source is also GPL.


