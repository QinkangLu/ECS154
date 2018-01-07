# Cache

## direct mapped cache
1. successfully created CacheLine, Cache, and Ram class

2. successfully implement the logic of write back and write allocate

3. successfully print out the correct data, hit, and dirty bit to console

3. failed to output the same data to the output file in format wanted !!


## set associative cache (8 ways)

1. Now I have already build CacheLine specifically for Direct Mapped Cache

2. But I can certainly make it more generic and adding some control parameters
	* for instance, DM cache is simply SA Cache that has its # of ways = # of lines
	* so we can have a parameter called num_ways that determine how are we going to setup our cache
	* this also means that our constructor needs to take in num_ways.

3. CacheSet is a new class that we need to add in
	* setWrite:
		* we go through a loop and look for the tag we want. If we found one, we have a write hit
		* if not, we have a miss. To deal with a write miss, we first need to find the Least Recently Used Cache Line that we want to replace with.
		* Then we need to check whether the dirty bit is set. If it is, we have to update our ram to make it coherent.
		* Then we bring the block with the tag we want to from ram.
		* Now we can safely overwrite the data at the proper offset
		* after we overwrite the data, we set the dirty bit to 1
		* since we only brought the block back not the whole line, we still need to set the tag

4. Ram should also be changed since the block that we are setting ot returning is going to be different
	* we used to set each block size to be 8 bytes but now after we change it to 8 way SA Cache, our block is only 4 bytes big.
	* So to make it more generic, we can set the parameter called block size in our Ram that help us to return the proper block size we want.