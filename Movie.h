#ifndef MOVIE_H
#define MOVIE_H

#include <QString>
#include <QStringList>

/**
 * @brief Represents a Bollywood movie entity in the system.
 */
class Movie {
public:
    Movie();
    Movie(int id,
          const QString &movieId,
          const QString &title,
          const QString &year,
          const QString &genre,
          const QString &overview,
          const QString &director,
          const QString &cast,
          double rating = -1.0,
          const QString &posterUrl = QString());

    // Getters
    int getId() const;
    QString getMovieId() const;
    QString getTitle() const;
    QString getYear() const;
    QString getGenre() const;
    QString getOverview() const;
    QString getDirector() const;
    QString getCast() const;
    double getRating() const;
    QString getPosterUrl() const;
    QString getSoup() const;

    // Setters
    void setId(int id);
    void setMovieId(const QString &movieId);
    void setTitle(const QString &title);
    void setYear(const QString &year);
    void setGenre(const QString &genre);
    void setOverview(const QString &overview);
    void setDirector(const QString &director);
    void setCast(const QString &cast);
    void setRating(double rating);
    void setPosterUrl(const QString &posterUrl);
    void setSoup(const QString &soup);

    // Helpers
    QString generateSoup() const;
    QString getFormattedYear() const;
    QString getDisplayRating() const;
    QStringList getGenreList() const;
    QStringList getCastList() const;
    bool isValid() const;

private:
    int m_id;
    QString m_movieId;
    QString m_title;
    QString m_year;
    QString m_genre;
    QString m_overview;
    QString m_director;
    QString m_cast;
    double m_rating;
    QString m_posterUrl;
    QString m_soup;
};

#endif // MOVIE_H
