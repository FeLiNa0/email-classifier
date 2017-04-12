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


def generate_files(out_fname_template, email_fnames,
                   include_html=True,  # use bs4
                   case_insensitive=False,  # call .lower()
                   n_gram_lengths=[1],
                   normalize_whitespace=True,  # call ' '.join(.split())
                   by_character=False  # call .split() or not
                   ):
    for i, eml in enumerate(email_fnames):
        if i % (len(email_fnames) // 10) == 0:
            print('Processing', eml)

        datum = parse_email(
            eml,
            include_html,
            case_insensitive,
            n_gram_lengths,
            normalize_whitespace,
            by_character)
        with open(out_fname_template.format(i), 'w') as f:
            f.write('// Derived from {}\n'.format(eml))
            for w in datum['payload']:
                f.write(w + '\n')


def ngrams(words, lengths, joiner=' '):
    return [joiner.join(words[i + j]
                        for j in range(n))
            for i, c in enumerate(words)
            for n in lengths if i + n < len(words)
            ]


def get_text(m):
    if m.is_multipart():
        return get_text(m.get_payload(0))
    else:
        return m.get_payload()


def parse_email(email_fname,
                include_html=True,
                case_insensitive=False,
                n_gram_lengths=[1, 2],
                normalize_whitespace=True,
                by_character=False
                ):

    # Open email file
    m = email.parser.BytesParser().parse(open(email_fname, 'rb'))
    datum = {'from': m['From'],
             'subject': m['Subject'],
             }

    payload = ''

    # If desired, strip payload of HTML tags
    if include_html:
        payload = get_text(m)
    else:
        payload = bs4.BeautifulSoup(get_text(m)).getText()

    # If desired, force case
    if case_insensitive:
        payload = payload.lower()

    # If desired, convert each sequence of whitespace to a single space
    if normalize_whitespace:
        payload = ' '.join(payload.split())

    # If desired, convert the payload into a list of whitespace separated words
    if not by_character:
        payload = payload.split()

    # Compute n-grams
    datum['payload'] = ngrams(payload, n_gram_lengths,
                              '' if by_character else ' ')

    # Check for missing fields
    for k in datum:
        if datum[k] is None:
            print(k, datum, fname)

    # Get useful metadata
    datum['date'] = m['Date']
    return datum


# Load 1000 emails from CSDMC2010 dataset
generate_files('data/payload{}.data', 
        ["spam/CSDMC2010_SPAM/TRAINING/TRAIN_{:05d}.eml".format(i) 
                    for i in range(1000)],
               include_html=False,
               case_insensitive=True,
               n_gram_lengths=[1, 2],
               by_character=False)
