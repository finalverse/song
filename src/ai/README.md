# Song AI module

This optional module is the provider-neutral boundary for AI capabilities in Finalverse Song Studio.

The module currently provides:

- typed, bounded AI requests and responses;
- a copyable request handle with cancellation, streaming chunks, and one terminal result;
- capability- and privacy-aware provider routing;
- a deterministic local provider for offline development and tests;
- a platform secret-store interface without an insecure fallback implementation;
- log redaction for common authorization headers, credential fields, and token shapes.

It deliberately does not contain a provider SDK, network endpoint, score mutation code, or persistent credential implementation. Future feature controllers should depend on `IAiService`, not a concrete model vendor. Providers must treat request context as private, avoid logging it, honor cancellation, and return only provider-neutral types.
