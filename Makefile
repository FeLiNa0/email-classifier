download_data:
	@echo download CSDMC2010_SPAM.zip
	@echo http://csmining.org/index.php/spam-email-datasets-.html
	@echo http://csmining.org/index.php/spam-email-datasets-.html?file=tl_files/Project_Datasets/task2/CSDMC2010_SPAM.zip


# Example:
# Write features from CSDMC emails numbered 0 to 100
# to .data files and create a CSV file containing data labels.
extract_features:
	extract_features.py \
		--ngram=5 --by-characte \
		--exclude-html --case-sensitive \
		CSDMC2010_SPAM/SPAMTrain.label CSDMC2010_SPAM/TRAINIG \
		'./metatadata_{}.data' './payload_{}.data' './labels0-100.csv' \
		0 100

# Example:
# Create a CSV file that contains all features from the
# given .data files in the given range.
# Generates vectors in the given dimension (128, 512, 1024, or 8192)
# Each vector is a bag-of-words where each word is hashed
# using Pearson's hashing function.
generate_matrix:
	generate_matrix \
		'./metadata_%d.data' './payload_%d.data' \
		'./data0-100.csv' 0 100 1024
