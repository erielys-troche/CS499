-- QuantigrationUpdates_original.sql
-- Original artifact from DAD 220 (Project One and Project Two).
--
-- These are the commands I originally ran by hand in the MySQL terminal
-- in Codio. The other practice queries from Project One (inserts, updates,
-- deletes, CSV export) were also run interactively and are not repeated here.
--
-- Weaknesses noted in my code review:
--   * every analysis query was run manually and screenshotted, so none of
--     it is reusable or automated
--   * the analysis queries are raw, ad hoc SQL (no stored procedures or views)
--   * filter/group columns like State and SKU have no index of their own

CREATE DATABASE QuantigrationUpdates;
USE QuantigrationUpdates;

-- ---------- Project One: schema ----------
CREATE TABLE Customers(
    CustomerID INT,
    FirstName VARCHAR(25),
    LastName VARCHAR(25),
    StreetAddress VARCHAR(50),
    City VARCHAR(50),
    State VARCHAR(25),
    ZipCode INT,
    Telephone VARCHAR(15),
    PRIMARY KEY (CustomerID)
);

CREATE TABLE Orders(
    OrderID INT,
    CustomerID INT,
    SKU VARCHAR(25),
    Description VARCHAR(100),
    PRIMARY KEY (OrderID),
    FOREIGN KEY (CustomerID) REFERENCES Customers(CustomerID)
);

CREATE TABLE RMA(
    RMAID INT,
    OrderID INT,
    Step VARCHAR(50),
    Status VARCHAR(15),
    Reason VARCHAR(25),
    PRIMARY KEY (RMAID),
    FOREIGN KEY (OrderID) REFERENCES Orders(OrderID)
);

-- ---------- Project One: load the CSV data ----------
LOAD DATA INFILE '/home/codio/workspace/customers.csv'
    INTO TABLE Customers
    FIELDS TERMINATED BY ','
    ENCLOSED BY '"'
    LINES TERMINATED BY '\r\n';

LOAD DATA INFILE '/home/codio/workspace/orders.csv'
    INTO TABLE Orders
    FIELDS TERMINATED BY ','
    ENCLOSED BY '"'
    LINES TERMINATED BY '\r\n';

LOAD DATA INFILE '/home/codio/workspace/rma.csv'
    INTO TABLE RMA
    FIELDS TERMINATED BY ','
    ENCLOSED BY '"'
    LINES TERMINATED BY '\r\n';

-- ---------- Project One: requirement change ----------
RENAME TABLE Customers TO Collaborator;

-- ---------- Project Two: analysis queries (run by hand) ----------
-- A. Number of returns by state
SELECT Collaborator.State, COUNT(*) AS AmountOfReturns
FROM Orders
INNER JOIN RMA ON Orders.OrderID = RMA.OrderID
INNER JOIN Collaborator ON Collaborator.CustomerID = Orders.CustomerID
GROUP BY State
ORDER BY AmountOfReturns DESC;

-- B. Percentage of returns by product type
SELECT
    SKU AS ProductSKU,
    Description AS ProductDescription,
    (COUNT(*) * 100 / (SELECT COUNT(*) FROM Orders WHERE Status = 'Complete')) AS ReturnsPercentage
FROM Orders
INNER JOIN RMA ON Orders.OrderID = RMA.OrderID
WHERE Status = 'Complete'
GROUP BY SKU, Description
ORDER BY ReturnsPercentage DESC;
