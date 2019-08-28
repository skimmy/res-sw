f=$1
n=$2
N=$3
smin=$4
# get file without extension
filename=$(basename -- "$f")
extension="${filename##*.}"
filename="${filename%.*}"

ONE=1
count=$(expr $n - $ONE)
echo $count

for s in `seq $smin $count`
do
    ../ged.o $f $n $N $s > ~/ed/ged/${filename}_n${n}_N${N}_s${s}.csv
done
