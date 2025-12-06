# CogLlamaX Component Specifications

This directory contains detailed specifications for each CogLlama component group.

## Quick Navigation

1. **[01-core.md](01-core.md)** - Core infrastructure (AtomSpace, CogServer, CogUtil)
2. **[02-perception.md](02-perception.md)** - Multi-modal sensory processing
3. **[03-knowledge.md](03-knowledge.md)** - Knowledge representation and reasoning (PLN, URE, Miner)
4. **[04-planning.md](04-planning.md)** - Hierarchical planning and spacetime reasoning
5. **[05-learning.md](05-learning.md)** - Continuous learning and adaptation (MOSES, Language Learning)
6. **[06-communication.md](06-communication.md)** - NLP and multi-agent communication
7. **[07-memory.md](07-memory.md)** - Memory management and attention allocation (ECAN)
8. **[08-tools.md](08-tools.md)** - External tool integration and ROS support
9. **[09-examples.md](09-examples.md)** - Complete code examples and usage patterns
10. **[10-performance.md](10-performance.md)** - Performance metrics, benchmarks, and testing

## Documentation Structure

```
.github/agents/
├── cogllama.md          # Quick reference (<30k chars)
├── cogllamax.md         # Master reference document
└── cogllamax/           # Detailed component specifications
    ├── README.md        # This file
    ├── 01-core.md
    ├── 02-perception.md
    ├── 03-knowledge.md
    ├── 04-planning.md
    ├── 05-learning.md
    ├── 06-communication.md
    ├── 07-memory.md
    ├── 08-tools.md
    ├── 09-examples.md
    └── 10-performance.md
```

## Usage

- Start with **[../cogllama.md](../cogllama.md)** for a quick overview
- Read **[../cogllamax.md](../cogllamax.md)** for the complete reference
- Dive into specific component files for detailed API specifications
- Check **[09-examples.md](09-examples.md)** for code examples
- Review **[10-performance.md](10-performance.md)** for benchmarks

## Contributing

When adding new components or APIs:

1. Update the relevant component file (01-08)
2. Add examples to 09-examples.md if applicable
3. Update performance targets in 10-performance.md
4. Ensure cogllama.md stays under 30k characters
5. Keep cogllamax.md synchronized with component files

## File Sizes

- cogllama.md: 12,243 chars (quick reference)
- cogllamax.md: 11,081 chars (master reference)
- Component files: 32,091 chars total (detailed specs)

Total documentation: ~55k chars across modular files
