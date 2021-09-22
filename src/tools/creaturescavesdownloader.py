# codec: utf-8

# https://www.creaturescaves.com/downloads.php?game=C3&species=&sortBy=ID&searchFor=&section=Creatures
# https://www.creaturescaves.com/downloads.php?section=Creatures&game=C3&species=&sortBy=ID&availability=&gender=&searchFor=&page=2

import html.parser
import re
import requests
import itertools
import os

class MyHTMLParser(html.parser.HTMLParser):
    def __init__(self):
        super().__init__()
        self.hrefs = []
    def handle_starttag(self, tag, attrs):
        if tag.lower() != 'a':
            return
        for (name, value) in attrs:
            if name.lower() != 'href':
                continue
            self.hrefs.append(value)

def main():
    try:
        os.mkdir("creaturescaves")
    except FileExistsError:
        pass
        
    for main_url in (
        "https://www.creaturescaves.com/downloads.php?game=C1&species=&sortBy=ID&searchFor=&section=Creatures",
        "https://www.creaturescaves.com/downloads.php?game=C2&species=&sortBy=ID&searchFor=&section=Creatures",
    ):
        
        for page in itertools.count(1):
            url = f"{main_url}&page={page}"
            print(f"Getting {url}")
            resp = requests.get(url)
            resp.raise_for_status()
            
            parser = MyHTMLParser()
            parser.feed(resp.text)
            
            downloads = []
            
            for h in parser.hrefs:
                if h.startswith("/downloads/"):
                    print(f"DOWNLOAD: {h}")
                    downloads.append(h)
            
            if not downloads:
                break
            
            for d in downloads:
                basename = os.path.basename(d)
                outname = f"creaturescaves/{basename}"
                if os.path.exists(outname):
                    continue
                
                print(f"Downloading {d}...")
                resp = requests.get("https://www.creaturescaves.com/" + d)
                with open("creaturescaves/temp", "wb") as f:
                    for chunk in resp.iter_content(chunk_size=4096):
                        if chunk:
                            f.write(chunk)
                os.rename("creaturescaves/temp", outname)

if __name__ == '__main__':
    main()