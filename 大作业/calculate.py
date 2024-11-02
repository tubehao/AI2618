# Given values
values = [
    469199.18, # HL, assuming typo, considering as H
    469210.71, # HL, assuming typo, considering as H
    46924.56,  # L
    469226.87, # HI, assuming typo, considering as H
    469224.56, # HI, assuming typo, considering as H
    469223.43, # HL, assuming typo, considering as H
    469209.59, # H
    46924.56,  # L
    469222.25, # H
    469210.71, # H
    4692343,   # H
    469222.25  # H
]

# Calculate the average
average_value = sum(values) / len(values)
print(average_value)
