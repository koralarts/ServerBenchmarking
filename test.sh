for(( i = 0; i < 3000; i++ ))
do
	./release/client -i 127.0.0.1 -l 1000 -t 10 &
done
