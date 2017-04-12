TOTAL=`sort $1 | uniq | wc -l`
UNIQ=`sort $1 | cut -d' ' -f1 | uniq | wc -l`

echo There is a total of $TOTAL unique words and a total of $UNIQ unique hash values in the file "$1".
echo There are $((TOTAL - UNIQ)) hash collisions.
