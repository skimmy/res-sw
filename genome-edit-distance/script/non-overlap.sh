f=$1
n=$2
N=$3
filename=$(basename -- "$f")
extension="${filename##*.}"
filename="${filename%.*}"
echo $filename

../ged.o $f $m $N 0 > ~/ed/ged/${filename}_n${n}_N${N}_s0.csv
