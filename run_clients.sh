#run: chmod +x run_clients.sh

# Number of clients to run simultaneously
num_clients=10

# Server address, server port, message size and message exchange count
server="borax.clear.rice.edu"
port=18000
size=65535
count=10000

# Loop to run multiple clients in parallel, save the results to .txt files
for i in $(seq 1 $num_clients); do
  echo "Starting client $i..."
  ./client_num $server $port $size $count > "client_output_$i.txt" 2>&1 & # Run the client with arguments in the background
  sleep 0.5  # Add a short delay between starting clients
done

# Wait for all background clients to finish
wait

echo "All clients have completed."
