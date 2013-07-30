import urllib2
from HTMLParser import HTMLParser
from htmlentitydefs import name2codepoint

class HASPParser(HTMLParser):
    def __init__(self):
        HTMLParser.__init__(self)
        self.useful = False

    def handle_starttag(self, tag, attrs):
        if tag == "table":
            self.useful = True
        
        elif tag == "a" and self.useful:
            for a in attrs:
                print base_url + a[1]

    def handle_endtag(self, tag):
        if tag == "table":
            self.useful = False

print

year = "2012"
payload = "11"

base_url = 'http://laspace.lsu.edu/hasp/groups/' + year + '/data/'

url = base_url + 'data.php?pname=Payload_' + payload + '&py=' + year

html = urllib2.urlopen(url)
lines = html.readlines()

p = HASPParser()
p.feed(''.join(lines))
