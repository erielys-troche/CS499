-- 01_database_enhancements.sql
-- ENHANCEMENT (Milestone Four — Databases)
--
-- Run this ONCE in MySQL (Codio terminal) after the original tables are
-- created and loaded:
--     mysql> SOURCE 01_database_enhancements.sql;
--
-- What this adds to the original artifact:
--   1. Indexes on the columns the analysis actually filters/groups by
--   2. A view for returns that are still open
--   3. Stored procedures for the report queries (one takes a parameter)
--   4. A least-privilege, read-only account for the Python report

USE QuantigrationUpdates;

-- ---------------------------------------------------------------------
-- 1. INDEXES
-- ---------------------------------------------------------------------
-- NOTE: I originally planned to index the foreign key columns
-- (Orders.CustomerID and RMA.OrderID), but DESCRIBE showed "MUL" in the
-- Key column for both -- MySQL (InnoDB) already creates an index on a
-- foreign key automatically. So I index the columns the queries use for
-- WHERE / GROUP BY instead.
--
-- Trade-off: each index speeds up reads on that column but adds a little
-- work to every INSERT/UPDATE/DELETE and uses extra storage. These columns
-- are read far more than they are written, so it's worth it.
--
-- (If you need to re-run this file, drop the indexes first:
--  DROP INDEX idx_collaborator_state ON Collaborator; etc.)
CREATE INDEX idx_collaborator_state ON Collaborator (State);
CREATE INDEX idx_orders_sku         ON Orders (SKU);
CREATE INDEX idx_rma_status         ON RMA (Status);

-- To check whether MySQL actually uses an index, run for example:
--   EXPLAIN SELECT State, COUNT(*) FROM Collaborator GROUP BY State;

-- ---------------------------------------------------------------------
-- 2. VIEW: returns that are not finished yet
-- ---------------------------------------------------------------------
-- Note: rows with a NULL Status are not included because NULL <> 'Complete'
-- is not true in SQL.
CREATE OR REPLACE VIEW ActiveReturnsView AS
SELECT
    r.RMAID,
    r.OrderID,
    r.Status,
    r.Step,
    r.Reason,
    o.SKU,
    c.State
FROM RMA r
INNER JOIN Orders o       ON o.OrderID = r.OrderID
INNER JOIN Collaborator c ON c.CustomerID = o.CustomerID
WHERE r.Status <> 'Complete';

-- ---------------------------------------------------------------------
-- 3. STORED PROCEDURES
-- ---------------------------------------------------------------------
DELIMITER $$

-- Returns by state (replaces the hand-typed query from Project Two)
DROP PROCEDURE IF EXISTS GetReturnsByState$$
CREATE PROCEDURE GetReturnsByState()
BEGIN
    SELECT c.State AS State, COUNT(*) AS AmountOfReturns
    FROM RMA r
    INNER JOIN Orders o       ON o.OrderID = r.OrderID
    INNER JOIN Collaborator c ON c.CustomerID = o.CustomerID
    GROUP BY c.State
    ORDER BY AmountOfReturns DESC;
END$$

-- Return rate by product: returns for a SKU divided by orders for that SKU.
-- (The Project Two version divided by a total instead of by each product's
-- own orders, which made the percentages hard to interpret.)
DROP PROCEDURE IF EXISTS GetReturnRateBySKU$$
CREATE PROCEDURE GetReturnRateBySKU()
BEGIN
    SELECT
        o.SKU AS SKU,
        MAX(o.Description) AS ProductDescription,
        COUNT(DISTINCT o.OrderID) AS TotalOrders,
        COUNT(r.RMAID) AS TotalReturns,
        ROUND(COUNT(r.RMAID) * 100.0 / COUNT(DISTINCT o.OrderID), 2) AS ReturnRatePct
    FROM Orders o
    LEFT JOIN RMA r ON r.OrderID = o.OrderID
    GROUP BY o.SKU
    ORDER BY ReturnRatePct DESC;
END$$

-- Returns by reason
DROP PROCEDURE IF EXISTS GetReturnsByReason$$
CREATE PROCEDURE GetReturnsByReason()
BEGIN
    SELECT COALESCE(Reason, 'Unknown') AS Reason, COUNT(*) AS AmountOfReturns
    FROM RMA
    GROUP BY COALESCE(Reason, 'Unknown')
    ORDER BY AmountOfReturns DESC;
END$$

-- Returns for ONE state, broken down by product. Takes a parameter, so the
-- state value is never pasted into the SQL text (prevents SQL injection).
DROP PROCEDURE IF EXISTS GetReturnsBySkuForState$$
CREATE PROCEDURE GetReturnsBySkuForState(IN p_state VARCHAR(25))
BEGIN
    SELECT o.SKU AS SKU, COUNT(*) AS AmountOfReturns
    FROM RMA r
    INNER JOIN Orders o       ON o.OrderID = r.OrderID
    INNER JOIN Collaborator c ON c.CustomerID = o.CustomerID
    WHERE c.State = p_state
    GROUP BY o.SKU
    ORDER BY AmountOfReturns DESC;
END$$

DELIMITER ;

-- ---------------------------------------------------------------------
-- 4. LEAST-PRIVILEGE ACCOUNT FOR THE PYTHON REPORT
-- ---------------------------------------------------------------------
-- The report only needs to run these procedures and read the view. It does
-- not need (and should not have) permission to change or delete data.
-- Stored procedures run with their creator's rights by default, so this
-- account never needs direct access to the tables.
--
-- CHANGE THE PASSWORD before running. Do not commit a real password to GitHub.
-- (This section is optional if your Codio account can't create users.)
CREATE USER IF NOT EXISTS 'rma_analyst'@'localhost' IDENTIFIED BY 'CHANGE_ME_BEFORE_USE';
GRANT EXECUTE ON PROCEDURE QuantigrationUpdates.GetReturnsByState       TO 'rma_analyst'@'localhost';
GRANT EXECUTE ON PROCEDURE QuantigrationUpdates.GetReturnRateBySKU      TO 'rma_analyst'@'localhost';
GRANT EXECUTE ON PROCEDURE QuantigrationUpdates.GetReturnsByReason      TO 'rma_analyst'@'localhost';
GRANT EXECUTE ON PROCEDURE QuantigrationUpdates.GetReturnsBySkuForState TO 'rma_analyst'@'localhost';
GRANT SELECT ON QuantigrationUpdates.ActiveReturnsView TO 'rma_analyst'@'localhost';
