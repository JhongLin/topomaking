#for seed in $(seq 1 9)
#do
	#echo "seed = $seed:"
	#./topo_generator 600 $seed
	#./getFlow.pl 70 $seed
	#./algo 1
	#./algo 2
	#./algo 3
#done

for seed in $(seq 1 20)
do
	./topo_generator 500 $seed
done
	
