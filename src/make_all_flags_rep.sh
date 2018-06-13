#takes as an input size of graphs to test, sparsity flag, alber flag
#Usage example:
# ./make_rep.sh 450 on none 
for sparsity in on off; do
  for alber in both; do
    echo $sparsity $alber;
    ./make_one_rep.sh $1 $sparsity $alber;
  done;
done