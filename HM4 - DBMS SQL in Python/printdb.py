import re
from persistence import *

def main():
    print("Activities")
    l1 = repo.activities.find_all_order('date')
    for item in l1:
        print("({}, {}, {}, '{}')".format(item.product_id,item.quantity,item.activator_id,item.date.decode()))
    print("Branches")
    l1 = repo.branches.find_all_order('id')
    for item in l1:
        print("({}, '{}', {})".format(item.id,item.location.decode(),item.number_of_employees))
    l1 = repo.employees.find_all_order('id')
    print("Employees")
    for item in l1:
        print("({}, '{}', {}, {})".format(item.id,item.name.decode(),item.salary,item.branche))
    l1 = repo.products.find_all_order('id')
    print("Products")
    for item in l1:
        print("({}, '{}', {}, {})".format(item.id,item.description.decode(),item.price,item.quantity))
    l1 = repo.suppliers.find_all_order('id')
    print("Suppliers")
    for item in l1:
        print("({}, '{}', '{}')".format(item.id,item.name.decode(),item.contact_information.decode()))
    print("\nEmployees report")
    emps_reps = repo.execute_command("""SELECT employees.name, employees.salary, branches.location, COALESCE(SUM(products.price * ABS(activities.quantity)),0) AS sales_income
    FROM employees
    LEFT JOIN branches ON employees.branche = branches.id
    LEFT JOIN activities ON employees.id = activities.activator_id
    LEFT JOIN products ON activities.product_id = products.id
    GROUP BY employees.id, employees.name, employees.salary, branches.location
    ORDER BY employees.name ASC
    """)
    for item in emps_reps:
        print("{} {} {} {}".format(item[0].decode(),item[1],item[2].decode(),item[3]))
    print("\nActivities report")
    acs_reps = repo.execute_command("""SELECT 
    activities.date, 
    products.description, 
    activities.quantity,
    employees.name as name_of_seller,
    suppliers.name as name_of_supplier
    FROM activities
    JOIN products ON activities.product_id = products.id
    LEFT JOIN employees ON activities.activator_id = employees.id 
    LEFT JOIN suppliers ON activities.activator_id = suppliers.id 
    ORDER BY date ASC
    """)
    for item in acs_reps:
        if(item[3] != None):
            print("('{}', '{}', {}, '{}', {})".format(item[0].decode(),item[1].decode(),item[2],item[3].decode(),item[4]))
        if(item[4] != None):
            print("('{}', '{}', {}, {}, '{}')".format(item[0].decode(),item[1].decode(),item[2],item[3],item[4].decode()))
if __name__ == '__main__':
    main()