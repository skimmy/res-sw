# Read Generator
This software can be used to generate reads in ``fasta`` and ``fastq`` format
from an input genome

## Synopsis
``read-gen genome reads length [error]``
### Options
``genome`` a ``fasta`` file with the genome from which create the read (mandatory)

``reads`` the number of reads to create (mandatory)

``length`` the length or the distribution of lengths of the reads (mandatory)

``error`` the type of error (optional)

## Examples

Creates 100 reads of length 50 from the genome in gen.fa an dsaves on reads.fasta
``read-gen gen.fa 100 50 > reads.fasta``