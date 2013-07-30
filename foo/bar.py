import xml.dom.minidom as minidom
import urllib2

def removeWhitespaceNodes(parent):
    """Removes nodes consisting of only whitespace from the DOM tree."""
    for child in list(parent.childNodes):
        if child.nodeType==3 and child.nodeValue.strip()=='':
            parent.removeChild(child)
        else:
            removeWhitespaceNodes(child)

# TODO: fix temporary
# TODO: generate 
url = "http://laspace.lsu.edu/hasp/groups/2012/data/data.php?pname=Payload_05&py=2012"

html = urllib2.urlopen(url)
lines = html.readlines()

# this line fails because of stupid non-standard html
root = minidom.parseString(''.join(lines))
removeWhitespaceNodes(root)

