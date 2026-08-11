# Song AI module

This optional module is the provider-neutral boundary for AI capabilities in Finalverse Song Studio.

The module currently provides:

- typed, bounded AI requests and responses;
- a copyable request handle with cancellation, streaming chunks, and one terminal result;
- capability- and privacy-aware provider routing;
- a versioned, selection-scoped score snapshot with deterministic JSON serialization;
- explicit note, lyric, and byte budgets with visible truncation metadata;
- a deterministic local provider for offline development and tests;
- a platform secret-store interface without an insecure fallback implementation;
- log redaction for common authorization headers, credential fields, and token shapes.

`IScoreContextBuilder` copies project, selection, instrumentation, tempo, meter, key, note, and lyric data synchronously from the active notation. The returned snapshot owns its data and retains no engraving pointers. It never assigns element IDs while reading a score; unavailable stable IDs and budget-driven omissions are reported explicitly.

The module deliberately does not contain a provider SDK, network endpoint, score mutation code, or persistent credential implementation. Future feature controllers should depend on `IAiService`, not a concrete model vendor. Providers must treat request context as private, avoid logging it, honor cancellation, and return only provider-neutral types.
