#from __future__ import annotations
from persistence import *


import sys
import os

def add_branche(splittedline : list):
    #TODO: add the branch into the repo
    id = splittedline[0]
    location = splittedline[1]
    number_of_employees =  splittedline[2]

    script = "INSERT INTO branches VALUES ({}, '{}', {});".format(id,location,number_of_employees) 
    repo.execute_command(script)
    
def add_supplier(splittedline : list):
    #TODO: insert the supplier into the repo
    id = splittedline[0]
    name = splittedline[1]
    contact_information =  splittedline[2]
    script = "INSERT INTO suppliers VALUES ({}, '{}', '{}');".format(id,name,contact_information) 
    repo.execute_command(script)

def add_product(splittedline : list):
    #TODO: insert product
    id = splittedline[0]
    description = splittedline[1]
    price =  splittedline[2]
    quantity =  splittedline[3]

    script = "INSERT INTO products VALUES ({}, '{}', {},{});".format(id,description,price,quantity) 
    repo.execute_command(script)

def add_employee(splittedline : list):
    #TODO: insert employee
    id = splittedline[0]
    name = splittedline[1]
    salary =  splittedline[2]
    branche =  splittedline[3]

    script = "INSERT INTO employees VALUES ({}, '{}', {},{});".format(id,name,salary,branche) 
    repo.execute_command(script)


adders = {  "B": add_branche,
            "S": add_supplier,
            "P": add_product,
            "E": add_employee}

def main(args : list):
    inputfilename = args[1]
    # delete the database file if it exists
    repo._close()
    # uncomment if needed
    if os.path.isfile("bgumart.db"):
        os.remove("bgumart.db")

    repo.__init__()
    repo.create_tables()
    with open(inputfilename) as inputfile:
        for line in inputfile:
            splittedline : list[str] = line.strip().split(",")
            adders.get(splittedline[0])(splittedline[1:])

if __name__ == '__main__':
    main(sys.argv)