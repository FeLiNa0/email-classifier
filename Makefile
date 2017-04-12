download_data:
	@echo download CSDMC2010_SPAM.zip
	@echo http://csmining.org/index.php/spam-email-datasets-.html
	@echo http://csmining.org/index.php/spam-email-datasets-.html?file=tl_files/Project_Datasets/task2/CSDMC2010_SPAM.zip

# Write features from CSDMC emails numbered 0 to 100
# to .data files and create a CSV file containing data labels.
features:
	@echo Sample parameters:
	python3 extract_features.py \
		--ngram=5 --by-character \
		--exclude-html --case-sensitive \
		CSDMC2010_SPAM/SPAMTrain.label CSDMC2010_SPAM/TRAINIG \
		'./metatadata{}.data' './payload{}.data' './labels0-100.csv' \
		0 100

# Compile matrix generator
generate_matrix: generate_matrix.c pearson.h
	gcc -O3 generate_matrix.c -o generate_matrix 

# Run `make matrix_data`
# Append all features from the given .data file to the CSV file.
# Generates vectors of the given dimension (128, 512, 1024, or 8192)
# where each vector is a bag-of-words and each word is hashed
# using Pearson's hashing function.
matrix_data: generate_matrix
	@echo Sample parameters:
	./generate_matrix 0 tests/test%d.data tests/out.csv 1024 1 3 reverse_hash

count_collisions:
	@echo Total unique words and total unique hash values in the file "reverse_hash" are
	sort reverse_hash | uniq | wc -l
	sort reverse_hash | cut -d' ' -f1 | uniq | wc -l
