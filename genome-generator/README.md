# Genome Generator
This software can be used to generate genomes in ``fasta`` format

## Synopsys

``genome-gen size [dist]``

### Options
``size`` represent the number of bases (mandatory)
``dist`` the distribution (optional)

- 1: GC-Rich
- 2: GC-Poor


## Examples

Generate an i.i.d. with uniform base distribution and 1000 bases

``genome-gen 1000``

Generate an i.d.d., GG-rich genome with 1000 bases and save on gcrich.fa

``genome-gen 1000 1 > gcrich.fa``

