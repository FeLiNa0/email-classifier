""" This file processes an .eml file and generates a Feature file of the
form: 

    >>> metadata_file, payload_file = generate_files(test_email)
    >>> print(payload_file)
    ... payload word 1
    ... ...
    ... payload word p
    
    Where a "word" is either a space-separated word n-gram or character n-gram.
    See the `parse_email` documentation for output options.  
"""
import email
import bs4


def generate_files(out_fname_template, email_fnames):
    for i, eml in enumerate(email_fnames):
        print('Processing', eml)
        datum = parse_email(eml)
        with open(out_fname_template.format(i), 'w') as f:
            f.write('// Derived from {}\n'.format(eml))
            for w in datum['payload']:
                f.write(w + '\n')


def ngrams(words, lengths):
    return [' '.join(words[i + j] 
            for j in range(n))
        for i, c in enumerate(words)
        for n in lengths if i + n < len(words)
    ]


def parse_email(email_fname,
    include_html=True,  # use bs4
    case_insensitive=False,  # call .lower()
    n_gram_lengths=[1,2],
    normalize_whitespace=True, # call ' '.join(.split())
    by_character=False  # call .split() or not
    ):

    # Open email file
    m = email.parser.BytesParser().parse(open(email_fname, 'rb'))
    datum = {'from': m['From'],
             'subject': m['Subject'],
    }

    # If desired, strip payload of HTML tags
    if include_html:
        datum['payload'] = m.get_payload()
    else:
        datum['payload'] = bs4.BeautifulSoup(m.get_payload()).getText()

    if normalize_whitespace:
        datum['payload'] = ' '.join(datum['payload'].split())

    if not by_character:
        datum['payload'] = datum['payload'].split()

    datum['payload'] = ngrams(datum['payload'], n_gram_lengths) 
    print(datum['payload'])

    # If desired, make all content lowercase
    if case_insensitive:
        for k in datum:
            if datum[k] is not None:
                datum[k] = datum[k].lower()

    # Check for missing fields
    for k in datum:
        if datum[k] is None:
            print(k, datum, fname)

    # Get useful metadata
    datum['date'] = m['Date']
    return datum


generate_files('payload{}.data', ["tests/test{}.eml".format(i) for i in [1,2,3]])
