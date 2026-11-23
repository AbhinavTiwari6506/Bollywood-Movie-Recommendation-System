-- ====================================================================
-- Bollywood Movie Recommendation System - MySQL Database Schema
-- Database: bollywood_recommender
-- Compatible with MySQL 8.0+
-- ====================================================================

-- Create Database if not exists
CREATE DATABASE IF NOT EXISTS bollywood_recommender
    CHARACTER SET utf8mb4
    COLLATE utf8mb4_unicode_ci;

USE bollywood_recommender;

-- Drop table if recreation is needed
-- DROP TABLE IF EXISTS movies;

-- Create Movies Table
CREATE TABLE IF NOT EXISTS movies (
    id INT AUTO_INCREMENT PRIMARY KEY,
    movie_id VARCHAR(50) NOT NULL UNIQUE COMMENT 'IMDb Movie Identifier (e.g., tt15354916)',
    movie_title VARCHAR(255) NOT NULL COMMENT 'Standardized Movie Title',
    release_year VARCHAR(20) DEFAULT NULL COMMENT 'Release Year (e.g., 2023)',
    genre VARCHAR(255) DEFAULT NULL COMMENT 'Comma-separated genres (e.g., Action, Thriller)',
    overview TEXT DEFAULT NULL COMMENT 'Plot synopsis and description',
    director VARCHAR(255) DEFAULT NULL COMMENT 'Director names',
    actors TEXT DEFAULT NULL COMMENT 'Lead cast and stars',
    rating DECIMAL(3, 1) DEFAULT NULL COMMENT 'IMDb Rating if available (0.0 to 10.0)',
    poster_url VARCHAR(500) DEFAULT NULL COMMENT 'URL or local path to poster image',
    soup TEXT DEFAULT NULL COMMENT 'Combined text metadata for TF-IDF vectorization',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP COMMENT 'Record insertion timestamp',
    
    -- Performance Indexes
    INDEX idx_movie_title (movie_title),
    INDEX idx_release_year (release_year),
    INDEX idx_genre (genre(100))
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci COMMENT='Bollywood Movies Metadata & Features';

-- Helpful views for analytics and dashboard
CREATE OR REPLACE VIEW v_popular_genres AS
SELECT 
    genre,
    COUNT(*) as movie_count
FROM movies
WHERE genre IS NOT NULL AND genre != ''
GROUP BY genre
ORDER BY movie_count DESC;

CREATE OR REPLACE VIEW v_top_directors AS
SELECT 
    director,
    COUNT(*) as movie_count
FROM movies
WHERE director IS NOT NULL AND director != ''
GROUP BY director
ORDER BY movie_count DESC;
