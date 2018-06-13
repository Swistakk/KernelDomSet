#takes as an input size of graphs to test, sparsity flag, alber flag
#Usage example:
# ./make_rep.sh 450 on none 
(for i in {1..100}; do ./ReduceGraph --in=../networks/planar$1.$i.txt --sparsity=$2 --alber=$3; done) > rep$1.$2.$3.txt