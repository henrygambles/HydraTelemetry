-- Query 1: Detect worker variance (Stragglers killing cluster efficiency)
SELECT 
    node_id,
    step_id,
    SUM(metric_value) AS total_step_duration_us,
    AVG(SUM(metric_value)) OVER(PARTITION BY step_id) as cluster_step_avg,
    SUM(metric_value) - AVG(SUM(metric_value)) OVER(PARTITION BY step_id) as worker_skew_us
FROM cluster_telemetry
WHERE metric_type = 'OP_DURATION_US'
GROUP BY node_id, step_id
HAVING worker_skew_us > 50000 -- Highlights nodes causing barrier delays
ORDER BY worker_skew_us DESC;

-- Query 2: Identify Dynamic Shape Allocation Overhead (XLA recompilation detection)
SELECT 
    operator_name,
    COUNT(DISTINCT tensor_shape) as distinct_runtime_shapes,
    AVG(metric_value) as average_execution_time_us
FROM cluster_telemetry
WHERE framework = 'JAX' AND metric_type = 'OP_DURATION_US'
GROUP BY operator_name
HAVING distinct_runtime_shapes > 1 -- Recompilation warning indicator
ORDER BY distinct_runtime_shapes DESC;
