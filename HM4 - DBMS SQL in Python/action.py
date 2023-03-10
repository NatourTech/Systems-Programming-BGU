#from __future__ import annotations
from persistence import *
from datetime import datetime


import sys

def main(args : list):
    inputfilename : str = args[1]
    with open(inputfilename) as inputfile:
        for line in inputfile:
            splittedline : list[str] = line.strip().split(", ")
            activite = Activitie(splittedline[0],splittedline[1],splittedline[2],splittedline[3])   
            #if you want to store date in this format: 01/jan/2023...
            # date_object = datetime.strptime(activite.date, "%Y%m%d")
            # date_string = date_object.strftime("%d/%b/%Y")
            # activite.date = date_string
            #(should deal with it when retreving data)
            if(int (activite.quantity) > 0):
                repo.activities.insert(activite)
                repo.execute_command("UPDATE products SET quantity = quantity + '{}' WHERE id = '{}'".format(activite.quantity,activite.product_id))
            elif(int (activite.quantity) < 0):
                l1 = repo.products.find(id=activite.product_id)
                x = l1[0]
                if(int(x.quantity) + int(activite.quantity) >= 0):
                    repo.activities.insert(activite)
                    repo.execute_command("UPDATE products SET quantity = quantity + '{}' WHERE id = '{}'".format(activite.quantity,activite.product_id))
                    if(int(x.quantity) + int(activite.quantity) == 0):
                        pass
                        #if quantity becomes 0 --> delete?
                        #repo.products.delete(id=activite.product_id)




if __name__ == '__main__':
    main(sys.argv)

