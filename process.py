import email
import bs4

def parse_email(fname, is_spam,
    include_html=True,  # use bs4
    case_insensitive=False,  # call .lower()
    n_gram_lens=[1,2],
    # TODO
    # [ [' '.join(words[i + j] for j in range(m)) for m in range(1,n+1)] for i, w in enumerate(words) if i < len(words)-n]
    # n = 3 ; ngrams = [ ' '.join(words[i + j] for j in range(m)) for m in range(1,n+1) for i, w in enumerate(words[:10]) if i < len(words[:10])-n]
    normalize_whitespace=True, # call ' '.join(.split())
    by_character=False  # call .split() or not
    ):

    # Open email file
    m = email.parser.BytesParser().parse(open(fname))
    datum = {'from': m['From'],
             'subject': m['Subject'],
    }

    # If desired, strip payload of HTML tags
    if include_html:
        datum['payload'] = m.get_payload()
    else:
        datum['payload'] = bs4.BeautifulSoup(m.get_payload()).getText()

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


