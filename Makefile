dimensions = 256 512 1024 2048 4096 8192
feature_names = word ngram
feature_dir = ../features
feature_dirs = $(addprefix ${feature_dir}/, $(addsuffix _features, ${feature_names}))

download_data:
	@echo download CSDMC2010_SPAM.zip
	@echo http://csmining.org/index.php/spam-email-datasets-.html
	@echo http://csmining.org/index.php/spam-email-datasets-.html?file=tl_files/Project_Datasets/task2/CSDMC2010_SPAM.zip

# Write features from CSDMC emails numbered 0 to 100
# to .features files and create a CSV file containing data labels.
features:
	mkdir -p ${feature_dirs}
	# Parameters are hardcoded 
	python3 extract_features.py

# Compile matrix generator
generate_matrix: generate_matrix.c pearson.h
	gcc -O3 cfarmhash/src/cfarmhash.c generate_matrix.c -o generate_matrix 

# Run `make matrix_data`
# Append all features from the given .features file to the CSV file.
# Generates vectors of all given dimension (128, 512, 1024, or 8192)
# where each vector is a bag-of-words and each word is hashed
# using Pearson's hashing function.
n ?= 1024
dense_matrix: generate_matrix
	./generate_matrix dense ${feature_dir}/word_features/%d.features ${feature_dir}/word_features/out$n.csv $n 0 4326 reverse_hash_word_$n ; \

all_dense_matrices: generate_matrix
	for d in ${feature_dirs}; do \
	for n in ${dimensions}; do \
	echo $$d $$n; \
	./generate_matrix dense $$d/%d.features $$d/out$$n.csv $$n 0 4326 $$d/reverse_hash$$n ; \
	done ; \
	done

# Set n to any number between 1 and the number of unique features in the dataset
sparse_matrix:
	./generate_matrix sparse data/payload%d.features data/out$$n.csv $$n 0 4326 reverse_hash$$n ; \

all_matrices: all_dense_matrices all_sparse_matrices

clean:
	rm -f generate_matrix *reverse_hash* ${feature_dir}/*/*.csv ${feature_dir}/*/*reverse_hash*

clean_all: clean
	rm -rf ${feature_dirs} 
